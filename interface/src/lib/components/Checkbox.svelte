<script lang="ts">
    export let label: string;
    export let value: boolean;
    export let onChange = () => {};
</script>

<label class="label cursor-pointer" for={label}>
    <span class="mr-4">{label}</span>
</label>
<input 
    type="checkbox"
    class="toggle toggle-primary"
    id={label}
    bind:checked={value}
    on:change={onChange}
/>