<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { addLed, colorLed, createScene } from './monitor';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import { socket } from '$lib/stores/socket';
	import type { MonitorData } from '$lib/types/models';
	import type { FixtureState } from '$lib/types/models';

	let el:HTMLCanvasElement;

	let fixtureState: FixtureState = { name: "test", lightsOn:true, brightness: 50, width: 16, height:16, depth:16, driverOn: true, monitorOn: true, pin: 16 };

	let done = false; //temp to show one instance of monitor data receiced

	onMount(() => {
		console.log("el", el)
		
		socket.on<FixtureState>('fixture', (data) => {
			console.log("Monitor on fixtureState", data);
			fixtureState = data;

			//rebuild scene
			createScene(el);

			for (let z = 0; z < fixtureState.depth; z++) {
				for (let y = 0; y < fixtureState.height; y++) {
					for (let x = 0; x < fixtureState.width; x++) {
						addLed(0.3, x - fixtureState.width/2, y - fixtureState.height/2, z - fixtureState.depth/2);
					}
				}
			}
		});

		//on receive data
		socket.on<MonitorData>('monitor', (data) => {
			if (!done)
				console.log("monitor data", data.uptime);

			let index2:any = 0	
			for (let x = 0; x < fixtureState.width; x++) {
				for (let y = 0; y < fixtureState.height; y++) {
					for (let z = 0; z < fixtureState.depth; z++) {
						let index = x + y * fixtureState.width + z * fixtureState.width * fixtureState.height;

						// if (!done) console.log("rgb", data.uptime[index2], data.uptime[index2+1], data.uptime[index2+2]);

						if (index2+2 < data.uptime.length)
							colorLed(index, data.uptime[index2]/255, data.uptime[index2+1]/255, data.uptime[index2+2]/255);
						else
							colorLed(index, 0,0,0);
						index2 += 3;
					}
				}
			}
			done = true;
		});
	});

	onDestroy(() => socket.off("monitor"));

</script>

<SettingsCard collapsible={true}>

	<span slot="title">Monitor</span>
	<!-- <div class="w-full overflow-x-auto"> -->
	 <div>
		<canvas bind:this={el}/>
		<!-- {$monitor.uptime} -->
	</div>

</SettingsCard>