<script lang="ts">
    export let label: string;
    export let onChange = (event:any) => {console.log(event)};
</script>

<label class="label cursor-pointer" for={label}>
    <span class="mr-4">{label}</span>
</label>
<input 
    type="file"
    id={label}
    on:change={onChange}
/>