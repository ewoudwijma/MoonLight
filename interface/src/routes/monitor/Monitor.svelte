<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { addLed, colorLed, createScene } from './monitor';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import { socket } from '$lib/stores/socket';
	import type { FixtureState } from '$lib/types/models';
	import ControlIcon from '~icons/tabler/adjustments';

	let el:HTMLCanvasElement;

	let fixtureState: FixtureState = { name: "test", lightsOn:true, brightness: 50, fixture: -1, width: 16, height:16, depth:16, driverOn: true, monitorOn: true, pin: 16 };
	let fixture: number = -1;
	let width = -1;
	let height = -1;
	let depth = -1;

	let done = false; //temp to show one instance of monitor data receiced

	const handleFixtureState = (data: FixtureState) => {
		console.log("Monitor.handleFixtureState", data.fixture, fixtureState.fixture);

		//check if fixture has changed
		if (data.fixture != fixture) {
			//rest api fixture definition
			// console.log("Monitor.handleFixtureState", data.fixture);
			//get data of fixture
			const loadFixtureDefinition = async () => {
				// try {
					const response = await fetch('/rest/fixdef?f=' + data.fixture, {
						method: 'GET',
						headers: {
							'Content-Type': 'application/json'
						}
					});
					let fixtureDefinitionState = await response.json();
					console.log("Monitor.handleFixtureState", fixtureDefinitionState, socket);
					if (fixtureDefinitionState.contents) {

						fixtureDefinitionState = JSON.parse(fixtureDefinitionState.contents);
					
						//rebuild scene
						createScene(el);

						width = 0;
						height = 0;
						depth = 0;
						
						//parse 1
						fixtureDefinitionState.outputs.forEach((output:any) => {
							output.leds.forEach((led: any) => {
								// console.log(led);
								// console.log(led.x / fixtureDefinitionState.factor - width/2, led.y / fixtureDefinitionState.factor - height/2, led.z / fixtureDefinitionState.factor - depth/2);
								let x = led[0] / fixtureDefinitionState.factor;
								let y = led[1] / fixtureDefinitionState.factor;
								let z = led[2] / fixtureDefinitionState.factor;

								if (x > width) width = x;
								if (y > height) height = y;
								if (z > depth) depth = z;

							});
						});
						width = Math.ceil(width) + 1;
						height = Math.ceil(height) + 1;
						depth = Math.ceil(depth) + 1;
						console.log(width, height, depth);
						//parse 2
						fixtureDefinitionState.outputs.forEach((output:any) => {
							output.leds.forEach((led: any) => {
								// console.log(led);
								// console.log(led.x / fixtureDefinitionState.factor - width/2, led.y / fixtureDefinitionState.factor - height/2, led.z / fixtureDefinitionState.factor - depth/2);
								let x = led[0] / fixtureDefinitionState.factor;
								let y = led[1] / fixtureDefinitionState.factor;
								let z = led[2] / fixtureDefinitionState.factor;

								addLed(0.3, x - width/2, y - height/2, z - depth/2);
							});
						});
					}
				// } catch (error) {
				// 	console.error('Error:', error);
				// }
			}
			loadFixtureDefinition();
			fixture = data.fixture;
		}

		fixtureState = data;
	};

	const handleMonitor = (data: Uint8Array) => {
		if (!done)
			console.log("Monitor.handleMonitor", data);

		for (let index = 0; index < data.length; index +=3) {
			colorLed(index/3, data[index]/255, data[index+1]/255, data[index+2]/255);
		}

		done = true;
	};

	onMount(() => {
		console.log("onMount Monitor", el)
		
		socket.on("fixture", handleFixtureState);

		//on receive data
		socket.on('monitor', handleMonitor);
	});

	onDestroy(() => {
		console.log("onDestroy Monitor");
		socket.off("fixture", handleFixtureState);
		socket.off("monitor", handleMonitor);
	});

</script>

<SettingsCard collapsible={false}>

	<ControlIcon slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Monitor</span>
	<!-- <div class="w-full overflow-x-auto"> -->
	 <div>
		<canvas bind:this={el}/>
	</div>

</SettingsCard>