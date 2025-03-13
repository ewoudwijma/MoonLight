<!--
   @title     MoonLight
   @file      Slider.svelte
   @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
   @Authors   https://github.com/MoonModules/MoonLight/commits/main
   @Copyright © 2025 Github MoonLight Commit Authors
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
-->

<script lang="ts">
    export let label: string;
    export let value: number;
    export let min = 0;
    export let max = 1;
    export let step = 0.01;
    export let onChange = () => {};
    export let hasNumber = true;
    export let disabled = false;
    // $: value = Math.round(value/step)*step;
</script>

<label class="label cursor-pointer" for={label}>
    <span class="mr-4">{label}</span>
</label>
<div class="flex-row flex items-center space-x-2">
    <input 
        type="range"
        min={min} 
        max={max}
        step={step}
        class={"range " + (disabled == false ? "range-primary" : "range-secondary")}
        id={label}
        {disabled}
        bind:value={value}
        on:change={onChange}
    />
    {#if hasNumber}
    <input 
        type="number"
        min={min} 
        max={max}
        step={step}
        class="input input-bordered"
        style="height: 2rem; width: 5rem"
        id={label}
        {disabled}
        bind:value={value}
        on:change={onChange}
    />
    {/if}
</div>