import { ref } from 'vue';

const testMode = ref(false);

export function useAppState() {
    return {
        testMode
    };
}
