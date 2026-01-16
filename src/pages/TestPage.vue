<template>
  <q-page class="test-page">
    <div class="gradient-background"></div>
    
    <div class="q-pa-md content-wrapper">
      <div class="text-h4 text-white text-center q-mb-lg dashboard-title">
        Test Mode Controls
      </div>

      <div class="row justify-center">
        <div class="col-12 col-md-8">
          <div class="glass-card">
            <div class="card-header q-mb-lg">
              <q-icon name="tune" size="28px" color="primary" />
              <span class="text-h6 q-ml-sm">Sensor Simulation</span>
            </div>

            <div class="test-controls text-white">
              <!-- BPM Control -->
              <div class="test-item q-mb-xl">
                <div class="test-label row items-center justify-between">
                  <div class="row items-center">
                    <q-icon name="favorite" size="28px" color="red-6" />
                    <span class="text-h6 q-ml-sm">Heart Rate (BPM)</span>
                  </div>
                  <div class="text-h5 text-weight-bold">{{ testBPM }} <span class="text-caption">BPM</span></div>
                </div>
                
                <q-slider
                  v-model="testBPM"
                  :min="40"
                  :max="200"
                  :step="1"
                  color="red-6"
                  track-size="8px"
                  thumb-size="25px"
                  label
                  class="q-mt-lg"
                />
              </div>

              <!-- SpO2 Control -->
              <div class="test-item q-mb-xl">
                <div class="test-label row items-center justify-between">
                  <div class="row items-center">
                    <q-icon name="air" size="28px" color="blue-6" />
                    <span class="text-h6 q-ml-sm">Blood Oxygen (SpO2)</span>
                  </div>
                  <div class="text-h5 text-weight-bold">{{ testSpO2 }} <span class="text-caption">%</span></div>
                </div>
                
                <q-slider
                  v-model="testSpO2"
                  :min="70"
                  :max="100"
                  :step="1"
                  color="blue-6"
                  track-size="8px"
                  thumb-size="25px"
                  label
                  class="q-mt-lg"
                />
              </div>

              <!-- AQI Control -->
              <div class="test-item q-mb-xl">
                <div class="test-label row items-center justify-between">
                  <div class="row items-center">
                    <q-icon name="cloud" size="28px" color="green-6" />
                    <span class="text-h6 q-ml-sm">Air Quality Index</span>
                  </div>
                  <div class="text-h5 text-weight-bold">{{ testAQI }} <span class="text-caption">AQI</span></div>
                </div>
                
                <q-slider
                  v-model="testAQI"
                  :min="0"
                  :max="500"
                  :step="1"
                  color="green-6"
                  track-size="8px"
                  thumb-size="25px"
                  label
                  class="q-mt-lg"
                />
              </div>

              <!-- Fall Status Toggle -->
              <div class="test-item q-mb-xl">
                <div class="test-label row items-center justify-between">
                  <div class="row items-center">
                    <q-icon name="warning" size="28px" color="orange-8" />
                    <span class="text-h6 q-ml-sm">Fall Detection</span>
                  </div>
                  <q-toggle
                    v-model="testFallStatus"
                    color="red"
                    size="lg"
                    icon="warning"
                    label="Trigger Fall Alert"
                    left-label
                  />
                </div>
              </div>

              <!-- Test Mode Enable/Disable -->
              <div class="test-item">
                <div class="test-label row items-center justify-between">
                  <div class="row items-center">
                    <q-icon name="science" size="28px" color="purple-6" />
                    <span class="text-h6 q-ml-sm">Test Mode Active</span>
                  </div>
                  <q-toggle
                    v-model="testModeEnabled"
                    color="purple"
                    size="lg"
                    icon="science"
                    label="Override Arduino"
                    left-label
                  />
                </div>
                <div class="text-caption text-grey-3 q-mt-sm">
                  {{ testModeEnabled ? '✓ Manual control active - Arduino updates paused' : '⚠ Arduino is controlling values' }}
                </div>
              </div>

            </div>
          </div>
          
          <div class="text-center q-mt-xl">
            <q-btn 
              to="/" 
              color="white" 
              text-color="primary" 
              label="Back to Dashboard" 
              rounded 
              size="lg" 
              icon="dashboard"
              class="glass-btn"
            />
          </div>
        </div>
      </div>
    </div>
  </q-page>
</template>

<script setup>
import { ref, watch, onMounted } from 'vue';
import { initializeApp } from 'firebase/app';
import { getDatabase, ref as dbRef, update } from 'firebase/database';

// Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyAdm0xR-NodxwUxtg2pY5QCgKd-hTTSKtY",
  authDomain: "sms2-ed4fb.firebaseapp.com",
  databaseURL: "https://sms2-ed4fb-default-rtdb.firebaseio.com",
  projectId: "sms2-ed4fb",
  storageBucket: "sms2-ed4fb.firebasestorage.app",
  messagingSenderId: "346913199350",
  appId: "1:346913199350:web:2f6db152dfa0ddd731c8d7",
  measurementId: "G-838BYV1HZ6"
};

// State
const testBPM = ref(75);
const testSpO2 = ref(98);
const testAQI = ref(50);
const testFallStatus = ref(false);
const testModeEnabled = ref(false);

let database = null;

// Initialize Firebase
onMounted(() => {
  try {
    const app = initializeApp(firebaseConfig);
    database = getDatabase(app);
  } catch (error) {
    console.error('Firebase init error', error);
  }
});

// Update Firebase when values change
const updateFirebase = () => {
  if (!database) return;
  
  update(dbRef(database, '/sensor_data'), {
    BPM: testBPM.value,
    SpO2: testSpO2.value,
    AQI: testAQI.value,
    Fall_Status: testFallStatus.value,
    test_mode: testModeEnabled.value  // Flag for Arduino to check
  });
};

watch([testBPM, testSpO2, testAQI, testFallStatus, testModeEnabled], () => {
  updateFirebase();
});
</script>

<style scoped>
.test-page {
  position: relative;
  min-height: 100vh;
}

.gradient-background {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 50%, #f093fb 100%);
  z-index: -1;
}

.content-wrapper {
  position: relative;
  z-index: 1;
}

.dashboard-title {
  font-weight: 700;
  text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.3);
}

.glass-card {
  background: rgba(255, 255, 255, 0.15);
  backdrop-filter: blur(10px);
  -webkit-backdrop-filter: blur(10px);
  border-radius: 20px;
  border: 1px solid rgba(255, 255, 255, 0.3);
  padding: 32px;
  box-shadow: 0 8px 32px 0 rgba(31, 38, 135, 0.37);
}

.card-header {
  display: flex;
  align-items: center;
  color: white;
  padding-bottom: 20px;
  border-bottom: 1px solid rgba(255, 255, 255, 0.2);
}

.test-item {
  background: rgba(0, 0, 0, 0.1);
  padding: 24px;
  border-radius: 16px;
}

.glass-btn {
  backdrop-filter: blur(5px);
  background: rgba(255, 255, 255, 0.9) !important;
}
</style>
