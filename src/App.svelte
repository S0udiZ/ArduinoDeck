<script lang="ts" async="true">
	import { onMount } from "svelte";
	import { writable, type Writable } from "svelte/store";
	import "./index.css";

	import emptyProfile from "./assets/emptyProfile.json";

	const emptyButton =
		"data:image/jpeg;base64,/9j/4AAQSkZJRgABAQEASABIAAD/2wBDAAEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQH/2wBDAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQH/wAARCAABAAEDAREAAhEBAxEB/8QAFAABAAAAAAAAAAAAAAAAAAAAC//EABQQAQAAAAAAAAAAAAAAAAAAAAD/xAAUAQEAAAAAAAAAAAAAAAAAAAAA/8QAFBEBAAAAAAAAAAAAAAAAAAAAAP/aAAwDAQACEQMRAD8AP/B//9k=";

	import {
		copyFile,
		BaseDirectory,
		writeTextFile,
		writeBinaryFile,
		readBinaryFile,
		readTextFile,
		createDir,
		exists,
		readDir,
	} from "@tauri-apps/api/fs";
	import { open } from "@tauri-apps/api/dialog";
	import { appDataDir, join } from "@tauri-apps/api/path";
	import { convertFileSrc } from "@tauri-apps/api/tauri";
	import { tauri } from "@tauri-apps/api";

	let appDataPath: string;

	const serial: Serial = navigator.serial;

	let connected: boolean = false;

	let disconnect: boolean = false;

	let port: SerialPort;

	let text: string = "";

	const selectedButton: Writable<SoundFile> = writable({} as SoundFile);

	type SoundFile = {
		id: string;
		sound: string;
		image: string;
		text: string;
		type: string;
	};

	let soundFiles: Array<SoundFile>;

	async function connectToPorts() {
		port = await serial.requestPort();
		await port.open({ baudRate: 9600 });
		connected = true;
		readFromArduino();
	}

	async function sendToArduino() {
		const encoder = new TextEncoderStream();
		const writer = encoder.writable.getWriter();
		writer.write("Hello World");
		writer.close();
	}

	async function readFromArduino() {
		const reader = port.readable.getReader();
		const decoder = new TextDecoder();
		while (true) {
			const { value, done } = await reader.read();
			if (done) {
				// Allow the serial port to be closed later.
				reader.releaseLock();
				break;
			}
			const readableValue = decoder.decode(value);
			text += readableValue;
			if (text.includes("\r\n")) {
				console.log(text);
				playSound(text.split("\r\n")[0]);
				text = "";
			}
		}
	}

	// function disconnectFromArduino() {
	//   if (port.readable.locked) {

	//   }
	//   port.close();
	//   connected = false;
	// }

	async function playSound(value: string) {
		const sound = soundFiles.find((soundFile) => soundFile.id == value);
		const filePath = await join(appDataPath, `assets/sounds/${sound.sound}`);
		const audio = new Audio(convertFileSrc(filePath));
		audio.play();
	}

	async function selectButton(button: SoundFile) {
		selectedButton.set(button);
	}

	async function handleImageChange() {
		const filePath: string = (await open({
			multiple: false,
			filters: [
				{
					name: "Images",
					extensions: ["png", "jpg", "jpeg", "webp"],
				},
			],
		})) as string;
		if (filePath === null) return;
		const fileName: string =
			filePath.split("\\")[filePath.split("\\").length - 1];
		await copyFile(filePath, `./assets/img/${fileName}`, {
			dir: BaseDirectory.App,
		});
		$selectedButton.image = fileName;
	}

	async function handleSoundChange() {
		const filePath: string = (await open({
			multiple: false,
			filters: [
				{
					name: "Sounds",
					extensions: ["mp3", "wav", "ogg"],
				},
			],
		})) as string;
		if (filePath === null) return;
		const fileName: string =
			filePath.split("\\")[filePath.split("\\").length - 1];
		await copyFile(filePath, `./assets/sounds/${fileName}`, {
			dir: BaseDirectory.App,
		});
		$selectedButton.sound = fileName;
	}

	async function saveChanges(e: Event) {
		e.preventDefault();
		const index = soundFiles.findIndex(
			(soundFile) => soundFile.id == $selectedButton.id
		);
		soundFiles[index] = $selectedButton;
		await writeTextFile("assets\\profile.json", JSON.stringify(soundFiles), {
			dir: BaseDirectory.App,
		});
	}

	async function showInFolder() {
		await tauri.invoke("show_in_folder", {
			path: (await join(appDataPath, "assets")).toString(),
		});
	}

	onMount(async () => {
		if (serial.getPorts()) {
			const ports = await serial.getPorts();
			if (ports.length > 0) {
				port = ports[0];
				await port.open({ baudRate: 9600 });
				connected = true;
				readFromArduino();
			}
		}

		appDataPath = await appDataDir();

		if (!(await exists("assets\\img", { dir: BaseDirectory.App }))) {
			await createDir("assets\\img", {
				dir: BaseDirectory.App,
				recursive: true,
			});
		}
		if (!(await exists("assets\\sounds", { dir: BaseDirectory.App }))) {
			await createDir("assets\\sounds", {
				dir: BaseDirectory.App,
				recursive: true,
			});
		}
		if (await exists("assets\\profile.json", { dir: BaseDirectory.App })) {
			soundFiles = JSON.parse(
				await readTextFile("assets\\profile.json", { dir: BaseDirectory.App })
			);
		} else {
			await writeTextFile(
				"assets\\profile.json",
				JSON.stringify(emptyProfile),
				{ dir: BaseDirectory.App }
			);
			soundFiles = emptyProfile as Array<SoundFile>;
		}
	});
</script>

<main class="max-w-lg mx-auto">
	<!-- {#if !connected} -->
	<button
		on:click={connectToPorts}
		class={"absolute top-2 right-2 p-1 px-2 border-2 border-white rounded-xl hover:border-blue-500" +
			" " +
			(connected ? "bg-green-500" : "bg-red-500")}
		disabled={connected}>Connect to arduino</button
	>
	<!-- {:else}
  <button on:click={disconnectFromArduino} class="absolute top-0 right-0"
    >Disconnect from arduino</button>
  {/if} -->
	<div class="w-full space-y-4">
		<div class="flex justify-evenly">
			{#if connected}
				<p>Connected</p>
			{:else}
				<p>Not connected</p>
			{/if}
			<button class="cursor-pointer" on:click={showInFolder}>
				Open file location 📁
			</button>
		</div>
		<div class="grid grid-cols-4 grid-rows-4 gap-2 max-w-fit mx-auto">
			{#if soundFiles}
				{#each soundFiles as soundFile}
					<button
						class="flex flex-col items-center justify-center relative cursor-pointer"
						on:click={() => selectButton(soundFile)}
					>
						<img
							src={soundFile.sound
								? convertFileSrc(
										`${appDataPath}assets\\img\\${soundFile.image}`
								  )
								: emptyButton}
							alt={soundFile.id}
							class="w-24 h-24 rounded-xl border-4 border-gray-400"
						/>
						<div
							class="absolute inset-0 bg-slate-100/40 hover:bg-transparent rounded-xl flex justify-center items-center"
						>
							<p class="text-black font-bold absolute">{soundFile.text}</p>
						</div>
					</button>
				{/each}
			{/if}
		</div>
		<div>
			{#if $selectedButton.id}
				<form
					class="border-2 border-white rounded-lg max-w-lg"
					on:submit={saveChanges}
				>
					<p>{$selectedButton.id}</p>
					<div class="relative w-fit h-fit inline-flex">
						<img
							src={$selectedButton.sound
								? convertFileSrc(
										`${appDataPath}assets\\img\\${$selectedButton.image}`
								  )
								: emptyButton}
							alt={$selectedButton.id}
							class="w-20 h-20 rounded-xl"
						/>
						<button
							type="button"
							on:click={handleImageChange}
							id=""
							class="absolute inset-0 opacity-0 hover:opacity-100 stroke-black stroke-2"
						>
							click to change image
						</button>
					</div>
					<input type="hidden" name="id" value={$selectedButton.id} />
					<label for="" class="inline-block w-32">
						Sound:
						<div class="relative w-fit h-fit">
							<input
								name="sound"
								value={$selectedButton.sound}
								disabled
								class="w-32"
							/>
							<button
								type="button"
								on:click={handleSoundChange}
								id=""
								class="absolute inset-0 opacity-10 hover:opacity-100 stroke-black stroke-2 text-right"
							>
								📁
							</button>
						</div>
					</label>
					<label for="" class="inline-block">
						Text:
						<br />
						<input name="text" bind:value={$selectedButton.text} />
					</label>
					<button
						type="submit"
						class="bg-blue-600 p-1 px-4 border border-white rounded-xl"
						>Save</button
					>
				</form>
			{:else}
				<p>No button selected</p>
			{/if}
		</div>
	</div>
</main>

<style>
	main {
		display: flex;
		flex-direction: column;
		align-items: center;
		justify-content: center;
	}
</style>
