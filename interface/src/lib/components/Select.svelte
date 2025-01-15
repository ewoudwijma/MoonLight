<script lang="ts">
    export let label: string;
    export let value: number;
    export let onChange = () => {};
</script>

<label class="label cursor-pointer" for={label}>
    <span class="mr-4">{label}</span>
</label>
<select 
    bind:value={value}
    on:change={onChange}
    class="select select-bordered w-full">
    <slot></slot>
</select>