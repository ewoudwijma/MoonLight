<!--
   @title     MoonLight
   @file      Button.svelte
   @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
   @Authors   https://github.com/MoonModules/MoonLight/commits/main
   @Copyright © 2025 Github MoonLight Commit Authors
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
-->

<script lang="ts">
    export let onClick: () => void;
    export let label: string;
</script>

<button class="btn btn-primary inline-flex items-center" on:click={onClick}
    ><slot></slot><span>{label}</span></button
>