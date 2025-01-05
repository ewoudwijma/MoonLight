<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import LightIcon from '~icons/tabler/bulb';
	import { socket } from '$lib/stores/socket';
	import type { FixtureState } from '$lib/types/models';
	import Slider from '$lib/components/Slider.svelte';
	import Checkbox from '$lib/components/Checkbox.svelte';
	import Number from '$lib/components/Number.svelte';

	let state: FixtureState = { name: "test", lightsOn:true, brightness: 50, width: 16, height:16, depth:16, driverOn: true, monitorOn: true, pin: 16 };

	//state is now via socket and not rest api ...
	
	async function getState() {
		try {
			const response = await fetch('/rest/fixtureState', {
				method: 'GET',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			const fixtureState = await response.json();
			state.lightsOn = fixtureState.lightsOn;
			state.brightness = fixtureState.brightness;
			state.width = fixtureState.width;
			state.height = fixtureState.height;
			state.depth = fixtureState.depth;
			state.driverOn = fixtureState.driverOn;
			state.monitorOn = fixtureState.monitorOn;
			state.pin = fixtureState.pin;
		} catch (error) {
			console.error('Error:', error);
		}
		return;
	}

	onMount(() => {
		socket.on<FixtureState>('fixture', (data) => {
			state = data;
		});
		// getState();
	});

	onDestroy(() => socket.off('fixture'));

	async function postState() {
		try {
			const response = await fetch('/rest/fixtureState', {
				method: 'POST',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify({ brightness: state.brightness, lightsOn: state.lightsOn })
			});
			if (response.status == 200) {
				notifications.success('Light state updated.', 3000);
				const fixtureState = await response.json();
				state.lightsOn = fixtureState.lightsOn;
				state.brightness = fixtureState.brightness;
				state.width = fixtureState.width;
				state.height = fixtureState.height;
				state.depth = fixtureState.depth;
				state.driverOn = fixtureState.driverOn;
				state.monitorOn = fixtureState.monitorOn;
				state.pin = fixtureState.pin;
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}
</script>

<SettingsCard collapsible={false}>
	<LightIcon slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Fixture</span>
	<div class="w-full">
		<Checkbox 
			label="On" 
			bind:value={state.lightsOn}
			onChange = {socket.sendEvent('fixture', state)}
		></Checkbox>
		<Slider 
			label="Brightness" 
			bind:value={state.brightness}
			min={0} 
			max={255} 
			step={1}
			onChange={socket.sendEvent('fixture', state)}
		></Slider>
		<Number 
			label="Width" 
			bind:value={state.width} 
			onChange = {socket.sendEvent('fixture', state)}
		></Number>
		<Number 
			label="Height" 
			bind:value={state.height} 
			onChange = {socket.sendEvent('fixture', state)}
		></Number>
		<Number 
			label="Depth" 
			bind:value={state.depth} 
			onChange = {socket.sendEvent('fixture', state)}
		></Number>
		<Checkbox 
			label="Driver on" 
			bind:value={state.driverOn}
			onChange = {socket.sendEvent('fixture', state)}
		></Checkbox>
		<Checkbox 
			label="Monitor on" 
			bind:value={state.monitorOn}
			onChange = {socket.sendEvent('fixture', state)}
		></Checkbox>
	
	</div>
</SettingsCard>
