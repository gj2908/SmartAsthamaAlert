<template>
  <q-page class="iot-dashboard">
    <!-- Professional Dark Gradient Background -->
    <div class="gradient-background"></div>

    <div class="q-pa-sm q-pa-md-md content-wrapper">
      <!-- Header Section -->
      <div class="header-section text-white q-mb-md row items-center justify-between">
        <div>
          <div class="text-overline text-grey-4 text-uppercase tracking-wider">Safety Monitor</div>
          <div class="text-h5 text-weight-light">Live Dashboard</div>
        </div>
        <div class="row items-center q-gutter-sm">
          <!-- Notification Bell -->
          <q-btn
            flat
            dense
            round
            :icon="notificationPermission === 'granted' ? 'notifications_active' : 'notifications_off'"
            :color="notificationPermission === 'granted' ? 'positive' : 'grey-5'"
            size="sm"
            @click="requestNotificationPermission"
          >
            <q-tooltip>
              {{ notificationPermission === 'granted' ? 'Notifications Enabled' : 'Enable Notifications' }}
            </q-tooltip>
            <q-badge v-if="notificationPermission === 'denied'" color="red" floating>!</q-badge>
          </q-btn>
          
          <!-- Connection Status -->
          <q-badge
            rounded
            :color="isConnected ? 'positive' : 'negative'"
            class="q-py-xs q-px-sm glass-badge"
          >
            <span :class="['status-dot', isConnected ? 'online' : 'offline']" class="q-mr-xs"></span>
            <q-icon :name="isConnected ? 'wifi' : 'wifi_off'" size="12px" class="q-mr-xs" />
            {{ isConnected ? 'Live' : 'Offline' }}
          </q-badge>
        </div>
      </div>

      <!-- Critical Alert Section (Conditional) -->
      <transition name="slide-fade">
        <div v-if="sensorData.Fall_Status" class="row q-mb-sm">
          <div class="col-12">
            <div class="alert-banner q-pa-sm">
              <div class="alert-content">
                <div class="alert-icon-wrapper">
                  <q-icon name="warning" class="alert-icon" size="sm" />
                </div>
                <div>
                  <div class="text-subtitle1 text-weight-bold">CRITICAL: FALL DETECTED</div>
                  <div class="text-subtitle2 opacity-80">Immediate assistance may be required. Sensor ID: #001</div>
                </div>
              </div>
              <q-btn flat dense icon="close" class="text-white" size="sm" />
            </div>
          </div>
        </div>
      </transition>

      <!-- Test Mode Indicator -->
      <transition name="slide-down">
        <div v-if="sensorData.test_mode" class="row q-mb-sm">
          <div class="col-12">
            <div class="test-mode-banner">
              <div class="row items-center justify-between">
                <div class="row items-center">
                  <q-icon name="science" color="purple-3" size="24px" class="q-mr-sm" />
                  <div>
                    <div class="text-subtitle2 text-weight-bold text-purple-2">TEST MODE ACTIVE</div>
                    <div class="text-caption text-purple-3">Arduino updates paused - Manual control enabled</div>
                  </div>
                </div>
                <q-btn 
                  flat 
                  dense 
                  label="Go to Test Page" 
                  icon="tune" 
                  color="purple-3" 
                  size="sm"
                  to="/test"
                />
              </div>
            </div>
          </div>
        </div>
      </transition>

      <div class="row q-col-gutter-md">
        <!-- Vitals Column -->
        <div class="col-12 col-md-8">
          <div class="row q-col-gutter-md">
            
            <!-- Heart Rate Card -->
            <div class="col-12 col-sm-6">
              <div class="pro-card vitals-card">
                <div class="card-top q-mb-sm">
                  <div class="icon-box bg-red-grad">
                    <q-icon name="favorite" size="xs" />
                  </div>
                  <div class="text-caption text-grey-4 text-weight-bold">HEART RATE</div>
                </div>
                
                <div class="card-main">
                  <div class="metric-value">
                    {{ sensorData.BPM }}
                    <span class="metric-unit"></span>
                  </div>
                  <div class="visualizer">
                    <div 
                      class="heart-dot"
                      :style="{ animationDuration: (60 / Math.max(sensorData.BPM, 40)) + 's' }"
                    ></div>
                    <div class="heart-waveform"></div>
                  </div>
                </div>

                <div class="card-footer q-mt-sm q-pt-sm">
                  <span :class="getBPMStatus(sensorData.BPM).color">
                    {{ getBPMStatus(sensorData.BPM).text }}
                  </span>
                  <span class="text-grey-5 text-caption float-right">Live</span>
                </div>
              </div>
            </div>

            <!-- SpO2 Card -->
            <div class="col-12 col-sm-6">
              <div class="pro-card vitals-card">
                <div class="card-top q-mb-sm">
                  <div class="icon-box bg-blue-grad">
                    <q-icon name="water_drop" size="xs" />
                  </div>
                  <div class="text-caption text-grey-4 text-weight-bold">OXYGEN %</div>
                </div>
                
                <div class="card-main">
                  <div class="metric-value">
                    {{ sensorData.SpO2 }}
                    <span class="metric-unit">%</span>
                  </div>
                  <div class="progress-bar-container">
                    <div class="progress-bar" :style="{ width: sensorData.SpO2 + '%' }"></div>
                  </div>
                </div>

                <div class="card-footer q-mt-sm q-pt-sm">
                  <span :class="getSpO2Status(sensorData.SpO2).color">
                    {{ getSpO2Status(sensorData.SpO2).text }}
                  </span>
                  <span class="text-grey-5 text-caption float-right">Sensor</span>
                </div>
              </div>
            </div>

            <!-- Environmental & Location Card -->
            <div class="col-12">
              <div class="pro-card aqi-card">
                <div class="row items-center">
                  <!-- Sensor/Indoor AQI -->
                  <div class="col-12 col-md-6 q-pa-sm text-center border-right">
                    <div class="text-caption text-grey-4 q-mb-xs">INDOOR AQI</div>
                    <q-circular-progress
                      show-value
                      :value="sensorData.AQI"
                      :max="500"
                      size="100px"
                      :thickness="0.15"
                      :color="getAQIColor(sensorData.AQI)"
                      track-color="transparent"
                      center-color="rgba(255,255,255,0.05)"
                      class="q-ma-sm"
                    >
                      <div class="column items-center">
                        <span class="text-h4 text-weight-light">{{ sensorData.AQI }}</span>
                      </div>
                    </q-circular-progress>
                    <div class="text-subtitle2" :class="'text-' + getAQIColor(sensorData.AQI)">
                      {{ getAQIStatus(sensorData.AQI) }}
                      <q-tooltip class="bg-dark text-body2" :offset="[0, 10]">
                        {{ getAQIDescription(sensorData.AQI) }}
                      </q-tooltip>
                    </div>
                    <div class="q-mt-xs text-center">
                      <span class="text-h6 text-weight-bold text-white">{{ sensorData.AQI }}</span>
                      <span class="text-caption text-grey-5 q-ml-xs">AQI</span>
                    </div>
                  </div>

                  <!-- Location Weather & Outdoor AQI -->
                  <div class="col-12 col-md-6 q-pa-sm">
                    <div class="row items-center justify-between q-mb-sm">
                      <div class="text-caption text-grey-4">OUTDOOR</div>
                      <q-btn flat round dense icon="edit_location" size="xs" @click="showLocationDialog = true">
                        <q-tooltip>Change Location</q-tooltip>
                      </q-btn>
                    </div>

                    <div class="location-box q-mb-sm">
                      <div class="row items-center">
                        <q-icon name="place" color="blue-4" size="16px" />
                        <span class="text-body2 q-ml-sm text-no-wrap ellipsis">{{ locationData.name }}</span>
                      </div>
                    </div>

                    <div class="row q-col-gutter-xs">
                      <div class="col-4">
                      <div class="weather-metrics text-center q-pa-xs">
                        <q-icon :name="getWeatherIcon(locationData.weatherCode)" size="20px" class="q-mb-xs" />
                        <div v-if="isLoadingData" class="flex justify-center">
                          <q-skeleton type="text" width="40px" height="20px" class="bg-grey-8" />
                        </div>
                        <div v-else class="text-h6 text-weight-light">{{ locationData.temp }}°C</div>
                        <div class="text-caption text-grey-5" style="font-size: 0.7rem">Temp</div>
                      </div>
                    </div>
                    <div class="col-4">
                      <div class="weather-metrics text-center q-pa-xs">
                        <q-icon name="cloud" size="20px" class="q-mb-xs text-grey-4" />
                        <div v-if="isLoadingData" class="flex justify-center">
                          <q-skeleton type="text" width="30px" height="20px" class="bg-grey-8" />
                        </div>
                        <div v-else class="text-h6 text-weight-light">{{ locationData.outdoorAQI }}</div>
                        <div class="text-caption text-grey-5" style="font-size: 0.7rem">India AQI</div>
                      </div>
                    </div>
                    <div class="col-4">
                      <div class="weather-metrics text-center q-pa-xs">
                        <q-icon name="water_drop" size="20px" class="q-mb-xs text-blue-4" />
                        <div v-if="isLoadingData" class="flex justify-center">
                          <q-skeleton type="text" width="30px" height="20px" class="bg-grey-8" />
                        </div>
                        <div v-else class="text-h6 text-weight-light">{{ locationData.humidity }}%</div>
                        <div class="text-caption text-grey-5" style="font-size: 0.7rem">Humidity</div>
                      </div>
                      </div>
                    </div>

                    <!-- Health Hazard Warning -->
                    <div class="q-mt-sm">
                      <div 
                        class="health-alert-box q-pa-xs rounded-borders"
                        :class="getAlertClass(locationData.outdoorAQI, locationData.temp)"
                      >
                        <div class="row items-start no-wrap">
                          <q-icon name="medical_services" size="20px" class="q-mr-sm q-mt-xs" />
                          <div class="text-caption text-weight-medium" style="line-height: 1.2; font-size: 0.75rem;">
                            {{ getHealthAdvice(locationData.outdoorAQI, locationData.temp, locationData.weatherCode) }}
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>

            <q-dialog v-model="showLocationDialog">
              <div class="glass-dialog q-pa-md" style="min-width: 350px; max-width: 90vw;">
                <div class="text-h6 text-white q-mb-md flex justify-between items-center">
                  Set Location
                  <div class="row items-center">
                    <q-btn 
                        flat 
                        dense 
                        color="light-blue-4" 
                        icon="my_location" 
                        label="Locate Me" 
                        class="q-mr-sm"
                        @click="handleLocateMe"
                        :loading="isLocating"
                    />
                    <q-btn flat round dense icon="close" v-close-popup />
                  </div>
                </div>
                
                <!-- Search Section -->
                <div class="q-mb-md">
                   <q-input 
                    dark 
                    filled 
                    v-model="searchQuery" 
                    label="Search City" 
                    placeholder="e.g. London, New York"
                    @keyup.enter="searchCity"
                    :loading="isSearching"
                    class="q-mb-sm"
                  >
                    <template v-slot:append>
                      <q-icon name="search" class="cursor-pointer" @click="searchCity" />
                    </template>
                  </q-input>

                  <!-- Search Results -->
                  <q-list dark separator bordered v-if="searchResults.length > 0" class="rounded-borders bg-dark-glass">
                    <q-item 
                      clickable 
                      v-ripple 
                      v-for="city in searchResults" 
                      :key="city.id"
                      @click="selectCity(city)"
                    >
                      <q-item-section avatar>
                        <q-icon name="place" color="primary" />
                      </q-item-section>
                      <q-item-section>
                        <q-item-label>{{ city.name }}</q-item-label>
                        <q-item-label caption class="text-grey-5">
                          {{ city.admin1 ? city.admin1 + ', ' : '' }}{{ city.country }}
                        </q-item-label>
                      </q-item-section>
                    </q-item>
                  </q-list>
                  <div v-else-if="!isSearching && searchQuery.length > 2" class="text-grey-5 text-center q-pa-sm">
                    Press enter to search...
                  </div>
                </div>

                 <q-separator dark class="q-my-md" />

                 <!-- Manual Fallback -->
                 <q-expansion-item
                    dark
                    icon="explore"
                    label="Manual Coordinates"
                    class="bg-transparent"
                 >
                  <div class="q-pa-sm">
                    <q-input dark filled v-model="manualLat" label="Latitude" class="q-mb-sm" />
                    <q-input dark filled v-model="manualLng" label="Longitude" class="q-mb-md" />
                    <div class="row justify-end">
                      <q-btn label="Update Manual" color="primary" flat @click="updateManualLocation" />
                    </div>
                  </div>
                 </q-expansion-item>
              </div>
            </q-dialog>
             
             <!-- History Dialog -->
             <q-dialog v-model="showHistory" maximized>
               <q-card dark class="bg-grey-9">
                 <q-card-section class="row items-center q-pb-none">
                   <div class="text-h6 text-white">Sensor History</div>
                   <q-space />
                   <q-btn icon="close" flat round dense v-close-popup />
                 </q-card-section>

                 <!-- Filters -->
                 <q-card-section class="q-pt-none">
                   <div class="row q-col-gutter-md">
                     <div class="col-12 col-md-4">
                       <q-input
                         dark
                         filled
                         v-model="dateFilter"
                         label="Filter by Date"
                         type="date"
                         clearable
                       >
                         <template v-slot:prepend>
                           <q-icon name="event" />
                         </template>
                       </q-input>
                     </div>
                     <div class="col-12 col-md-4">
                       <q-input
                         dark
                         filled
                         v-model="timeFilterStart"
                         label="Start Time"
                         type="time"
                         clearable
                       >
                         <template v-slot:prepend>
                           <q-icon name="schedule" />
                         </template>
                       </q-input>
                     </div>
                     <div class="col-12 col-md-4">
                       <q-input
                         dark
                         filled
                         v-model="timeFilterEnd"
                         label="End Time"
                         type="time"
                         clearable
                       >
                         <template v-slot:prepend>
                           <q-icon name="schedule" />
                         </template>
                       </q-input>
                     </div>
                   </div>
                 </q-card-section>

                 <q-card-section>
                   <q-table
                     dark
                     :rows="filteredHistory"
                     :columns="historyColumns"
                     row-key="id"
                     :rows-per-page-options="[10, 20, 50, 100]"
                     :pagination="{ rowsPerPage: 20, sortBy: 'timestamp', descending: true }"
                     class="bg-transparent"
                   >
                     <template v-slot:body-cell-timestamp="props">
                       <q-td :props="props">
                         <div class="text-caption">{{ formatDate(props.row.timestamp) }}</div>
                         <div class="text-caption text-grey-5">{{ formatTime(props.row.timestamp) }}</div>
                       </q-td>
                     </template>
                     <template v-slot:body-cell-Fall_Status="props">
                       <q-td :props="props">
                         <q-badge :color="props.row.Fall_Status ? 'red' : 'green'">
                           {{ props.row.Fall_Status ? 'YES' : 'NO' }}
                         </q-badge>
                       </q-td>
                     </template>
                     <template v-slot:body-cell-AQI="props">
                       <q-td :props="props">
                         <span :class="'text-' + getAQIColor(props.row.AQI)">
                           {{ props.row.AQI }}
                         </span>
                       </q-td>
                     </template>
                   </q-table>
                 </q-card-section>
               </q-card>
             </q-dialog>
           </div>
        </div>
        <div class="col-12 col-md-4">
          <div class="pro-card full-height column justify-between">
            <div>
              <div class="text-overline text-grey-5 q-mb-lg">DEVICE DIAGNOSTICS</div>
              
              <q-list dark padding separator>
                <q-item>
                  <q-item-section avatar>
                    <q-icon name="memory" color="grey-5" />
                  </q-item-section>
                  <q-item-section>
                    <q-item-label>Sensor Status</q-item-label>
                    <q-item-label caption class="text-grey-6">All systems nominal</q-item-label>
                  </q-item-section>
                  <q-item-section side>
                    <q-icon name="check_circle" color="positive" size="xs" />
                  </q-item-section>
                </q-item>

                <q-item>
                  <q-item-section avatar>
                    <q-icon name="access_time" color="grey-5" />
                  </q-item-section>
                  <q-item-section>
                    <q-item-label>Last Updated</q-item-label>
                    <q-item-label caption class="text-grey-6">{{ lastUpdated }}</q-item-label>
                  </q-item-section>
                </q-item>

                <q-item>
                  <q-item-section avatar>
                    <q-icon name="battery_full" color="grey-5" />
                  </q-item-section>
                  <q-item-section>
                    <q-item-label>Power Supply</q-item-label>
                    <q-item-label caption class="text-grey-6">Mains Connected</q-item-label>
                  </q-item-section>
                </q-item>
              </q-list>
              
              <!-- History & SD Card Actions -->
              <div class="q-mt-md q-gutter-sm">
                <q-btn 
                  outline 
                  color="blue-4" 
                  icon="history" 
                  label="View History" 
                  size="sm"
                  class="full-width"
                  @click="showHistory = true"
                />
                <q-btn 
                  outline 
                  color="orange-4" 
                  icon="sd_card" 
                  label="SD Card Logs" 
                  size="sm"
                  class="full-width"
                  @click="openSDCard"
                />
              </div>
            </div>

            <div class="q-mt-md text-center">
              <div class="text-caption text-grey-7">Safety Monitor v1.0</div>
            </div>
          </div>
        </div>
      </div>
    </div>

  </q-page>
</template>

<script setup>
import { ref, onMounted, onUnmounted, computed } from 'vue';
import { initializeApp } from 'firebase/app';
import { getDatabase, ref as dbRef, onValue, off, query, limitToLast } from 'firebase/database';
import { getAnalytics } from 'firebase/analytics';
import { useEmergencyContacts } from '../composables/useEmergencyContacts';

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

// Reactive data
const sensorData = ref({
  AQI: 0,
  Fall_Status: false,
  BPM: 0,
  SpO2: 0
});

const isConnected = ref(false);
const lastUpdated = ref('Connecting...');
const notificationPermission = ref('Notification' in window ? Notification.permission : 'denied');
const locationData = ref({
  lat: null,
  lng: null,
  name: 'Locating...',
  temp: '--',
  weatherCode: 0,
  outdoorAQI: '--',
  humidity: '--'
});
const showLocationDialog = ref(false);
const searchQuery = ref('');
const searchResults = ref([]);
const isSearching = ref(false);
const isLocating = ref(false);
const isLoadingData = ref(true);
const manualLat = ref('');
const manualLng = ref('');

// History Feature
const historyList = ref([]);
const showHistory = ref(false);
const dateFilter = ref('');
const timeFilterStart = ref('');
const timeFilterEnd = ref('');
const arduinoIP = "http://192.168.1.15"; // Update with your Arduino IP

// Emergency Contacts
const { triggerEmergencyProtocol, hasContacts } = useEmergencyContacts();

const historyColumns = [
  { name: 'timestamp', label: 'Time', field: 'timestamp', align: 'left', sortable: true },
  { name: 'AQI', label: 'AQI', field: 'AQI', align: 'center', sortable: true },
  { name: 'BPM', label: 'Heart Rate', field: 'BPM', align: 'center', sortable: true },
  { name: 'SpO2', label: 'SpO2 (%)', field: 'SpO2', align: 'center', sortable: true },
  { name: 'Fall_Status', label: 'Fall Detected', field: 'Fall_Status', align: 'center', sortable: true }
];

let database = null;
let sensorDataRef = null;
let historyRef = null;

// Initialize
onMounted(() => {
  initFirebase();
  detectLocation();
  requestNotificationPermission();
});

// Notification System
const requestNotificationPermission = async () => {
  if (!('Notification' in window)) {
    window.$q?.notify({
      type: 'warning',
      message: 'Notifications not supported in this browser',
      position: 'top',
      timeout: 3000
    });
    return;
  }

  if (Notification.permission === 'granted') {
    window.$q?.notify({
      type: 'positive',
      message: 'Notifications are already enabled',
      position: 'top',
      timeout: 2000
    });
    return;
  }

  try {
    const permission = await Notification.requestPermission();
    notificationPermission.value = permission;
    console.log('Notification permission:', permission);
    
    if (permission === 'granted') {
      window.$q?.notify({
        type: 'positive',
        message: '✓ Emergency notifications enabled',
        caption: 'You will be alerted if a fall is detected',
        position: 'top',
        timeout: 3000,
        icon: 'notifications_active'
      });
    } else if (permission === 'denied') {
      window.$q?.notify({
        type: 'negative',
        message: 'Notification permission denied',
        caption: 'Enable in browser settings to receive alerts',
        position: 'top',
        timeout: 5000,
        icon: 'notifications_off'
      });
    }
  } catch (error) {
    console.error('Notification permission error:', error);
  }
};

const sendFallNotification = () => {
  console.log('🚨 sendFallNotification called');
  console.log('Notification permission:', Notification.permission);
  
  // Browser Notification
  if ('Notification' in window) {
    console.log('Notification API available');
    
    if (Notification.permission === 'granted') {
      console.log('Creating notification...');
      try {
        const notification = new Notification('🚨 EMERGENCY: Fall Detected!', {
          body: 'A fall has been detected. Immediate assistance may be required.',
          icon: '/favicon.ico',
          tag: 'fall-alert',
          requireInteraction: true,
          vibrate: [200, 100, 200, 100, 200]
        });

        notification.onclick = () => {
          console.log('Notification clicked');
          window.focus();
          notification.close();
        };
        
        console.log('✓ Browser notification created');
      } catch (error) {
        console.error('Notification creation error:', error);
      }
    } else {
      console.warn('Notification permission not granted:', Notification.permission);
      // Show in-app alert if permission not granted
      alert('🚨 FALL DETECTED! Please enable notifications for emergency alerts.');
    }
  } else {
    console.warn('Notification API not available');
  }

  // Sound Alert
  console.log('Playing alert sound...');
  playAlertSound();

  // Vibrate (mobile)
  if ('vibrate' in navigator) {
    console.log('Triggering vibration...');
    navigator.vibrate([200, 100, 200, 100, 200, 100, 200]);
  }

  // Quasar Notification (always show this as fallback)
  console.log('Showing Quasar notification...');
  window.$q?.notify({
    type: 'negative',
    message: '🚨 FALL DETECTED!',
    caption: 'Emergency assistance may be required',
    position: 'top',
    timeout: 0, // Persistent
    actions: [
      { label: 'Dismiss', color: 'white', handler: () => {} }
    ],
    icon: 'warning',
    color: 'red-9'
  });
  
  // Trigger Emergency Contact Protocol
  console.log('Triggering emergency contact protocol...');
  setTimeout(async () => {
    try {
      const result = await triggerEmergencyProtocol();
      console.log('Emergency protocol result:', result);
      
      if (result.success && result.actions.length > 0) {
        window.$q?.notify({
          type: 'info',
          message: '📞 Emergency Protocol Activated',
          caption: result.message,
          position: 'bottom',
          timeout: 8000,
          icon: 'phone',
          color: 'blue-9'
        });
      } else if (!hasContacts.value) {
        window.$q?.notify({
          type: 'warning',
          message: 'No emergency contacts configured',
          caption: 'Add contacts in Settings to enable auto-call/SMS',
          position: 'bottom',
          timeout: 5000,
          icon: 'person_add',
          color: 'orange-9',
          actions: [
            { label: 'Settings', color: 'white', handler: () => {
              window.location.href = '/settings';
            }}
          ]
        });
      }
    } catch (error) {
      console.error('Emergency protocol error:', error);
    }
  }, 500);
  
  console.log('✓ All notification channels triggered');
};

const playAlertSound = () => {
  try {
    const audio = new Audio('data:audio/wav;base64,UklGRnoGAABXQVZFZm10IBAAAAABAAEAQB8AAEAfAAABAAgAZGF0YQoGAACBhYqFbF1fdJivrJBhNjVgodDbq2EcBj+a2/LDciUFLIHO8tiJNwgZaLvt559NEAxQp+PwtmMcBjiR1/LMeSwFJHfH8N2QQAoUXrTp66hVFApGn+DyvmwhBSuBzvLZiTYIGGS57OihUBELTKXh8bllHAU2jdXvzn0pBSh+zPDajzsKElyx6OyrWBUIQ5zd8sFuJAUuhM/z1YU4BxlnuOvpoVIRC0yl4fG5ZRwFNo3V7859KQUofsz');
    audio.volume = 0.5;
    audio.play().catch(e => console.log('Audio play failed:', e));
  } catch (error) {
    console.error('Alert sound error:', error);
  }
};

const initFirebase = () => {
  try {
    const app = initializeApp(firebaseConfig);
    getAnalytics(app); 
    database = getDatabase(app);
    sensorDataRef = dbRef(database, '/sensor_data');
    
    let previousFallStatus = false;
    
    // Listen to live sensor data
    onValue(sensorDataRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        const newFallStatus = data.Fall_Status || false;
        
        console.log('Firebase data received:', {
          Fall_Status: newFallStatus,
          previousFallStatus: previousFallStatus,
          willTrigger: newFallStatus && !previousFallStatus
        });
        
        sensorData.value = {
          AQI: data.AQI || 0,
          Fall_Status: newFallStatus,
          BPM: data.BPM || 0,
          SpO2: data.SpO2 || 0,
          test_mode: data.test_mode || false
        };
        
        // Trigger notification on fall detection (edge detection)
        if (newFallStatus && !previousFallStatus) {
          console.log('🚨 Fall detected - sending notification');
          console.log('Previous:', previousFallStatus, '→ New:', newFallStatus);
          sendFallNotification();
        } else if (newFallStatus) {
          console.log('Fall status still true (no edge)');
        }
        
        previousFallStatus = newFallStatus;
        isConnected.value = true;
        const now = new Date();
        lastUpdated.value = now.toLocaleTimeString();
      }
    }, (error) => {
      console.error('Firebase error:', error);
      isConnected.value = false;
    });
    
    // Listen to historical data (last 20 entries)
    historyRef = query(dbRef(database, 'history'), limitToLast(20));
    onValue(historyRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        // Convert Firebase object to array and reverse (newest first)
        historyList.value = Object.keys(data).map(key => ({
          id: key,
          ...data[key]
        })).reverse();
      }
    });
    
  } catch (error) {
    console.error('Firebase initialization error:', error);
    isConnected.value = false;
  }
};

onUnmounted(() => {
  if (sensorDataRef && database) {
    off(sensorDataRef);
  }
  if (historyRef && database) {
    off(historyRef);
  }
});

// History Functions
const openSDCard = () => {
  window.open(arduinoIP, '_blank');
};

// Location & Weather Logic
const handleLocateMe = () => {
    isLocating.value = true;
    detectLocation();
};

const detectLocation = () => {
  if (navigator.geolocation) {
    // Request high accuracy position with timeout
    const options = {
      enableHighAccuracy: true,
      timeout: 10000,  // 10 second timeout
      maximumAge: 0    // Don't use cached position
    };
    
    navigator.geolocation.getCurrentPosition(
      (position) => {
        console.log("GPS Position:", position.coords);
        locationData.value.lat = position.coords.latitude;
        locationData.value.lng = position.coords.longitude;
        locationData.value.name = "Locating...";
        fetchWeather(position.coords.latitude, position.coords.longitude);
        isLocating.value = false;
        showLocationDialog.value = false;
        
        // Success notification
        window.$q?.notify({
          type: 'positive',
          message: 'Location detected successfully!',
          position: 'top',
          timeout: 2000
        });
      },
      async (error) => {
        console.error("Geolocation error:", error);
        
        let errorMessage = '';
        switch(error.code) {
          case error.PERMISSION_DENIED:
            errorMessage = "Location permission denied. Using IP-based location...";
            break;
          case error.POSITION_UNAVAILABLE:
            errorMessage = "Location unavailable. Using IP-based location...";
            break;
          case error.TIMEOUT:
            errorMessage = "Location request timeout. Using IP-based location...";
            break;
          default:
            errorMessage = "Location error. Using IP-based location...";
        }
        
        window.$q?.notify({
          type: 'warning',
          message: errorMessage,
          position: 'top',
          timeout: 3000
        });
        
        // Fallback to IP-based location
        await detectLocationByIP();
      },
      options
    );
  } else {
    console.warn("Geolocation not supported");
    window.$q?.notify({
      type: 'warning',
      message: 'GPS not supported. Using IP-based location...',
      position: 'top',
      timeout: 3000
    });
    detectLocationByIP();
  }
};

// IP-based location fallback
const detectLocationByIP = async () => {
  try {
    const response = await fetch('https://ipapi.co/json/');
    const data = await response.json();
    
    if (data.latitude && data.longitude) {
      console.log("IP Location:", data);
      locationData.value.lat = data.latitude;
      locationData.value.lng = data.longitude;
      locationData.value.name = `${data.city}, ${data.region}`;
      fetchWeather(data.latitude, data.longitude);
      
      window.$q?.notify({
        type: 'info',
        message: `Location set to: ${data.city}`,
        position: 'top',
        timeout: 2000
      });
    } else {
      throw new Error("No location data");
    }
  } catch (error) {
    console.error("IP location error:", error);
    locationData.value.name = "Location Detection Failed";
    
    window.$q?.notify({
      type: 'negative',
      message: 'Could not detect location. Please search manually.',
      position: 'top',
      timeout: 3000
    });
  } finally {
    isLocating.value = false;
  }
};

const searchCity = async () => {
  if (!searchQuery.value || searchQuery.value.length < 2) return;
  
  isSearching.value = true;
  try {
    const response = await fetch(`https://geocoding-api.open-meteo.com/v1/search?name=${encodeURIComponent(searchQuery.value)}&count=5&language=en&format=json`);
    const data = await response.json();
    searchResults.value = data.results || [];
  } catch (error) {
    console.error("Search failed", error);
    searchResults.value = [];
  } finally {
    isSearching.value = false;
  }
};

const selectCity = (city) => {
  locationData.value.lat = city.latitude;
  locationData.value.lng = city.longitude;
  locationData.value.name = `${city.name}, ${city.country || ''}`;
  
  fetchWeather(city.latitude, city.longitude);
  
  searchQuery.value = '';
  searchResults.value = [];
  showLocationDialog.value = false;
};

const updateManualLocation = () => {
  if (manualLat.value && manualLng.value) {
    locationData.value.lat = parseFloat(manualLat.value);
    locationData.value.lng = parseFloat(manualLng.value);
    fetchWeather(locationData.value.lat, locationData.value.lng);
    showLocationDialog.value = false;
  }
};

const fetchWeather = async (lat, lng) => {
  isLoadingData.value = true;
  try {
    const waqiToken = '908f8533a9457071f16107ea95faef85052a4eb8';
    
    // Fetch from both APIs in parallel
    const waqiUrl = `https://api.waqi.info/feed/geo:${lat};${lng}/?token=${waqiToken}`;
    const weatherUrl = `https://api.open-meteo.com/v1/forecast?latitude=${lat}&longitude=${lng}&current=temperature_2m,relative_humidity_2m,weather_code&timezone=auto`;
    
    const [waqiRes, weatherRes] = await Promise.all([
      fetch(waqiUrl),
      fetch(weatherUrl)
    ]);
    
    const waqiData = await waqiRes.json();
    const weatherData = await weatherRes.json();
    
    console.log("WAQI Response:", waqiData);
    console.log("Open-Meteo Response:", weatherData);
    
    // 1. Get AQI from WAQI (Official monitoring stations)
    if (waqiData.status === 'ok' && waqiData.data) {
      locationData.value.outdoorAQI = waqiData.data.aqi || 'N/A';
    } else {
      locationData.value.outdoorAQI = 'N/A';
    }
    
    // 2. Get Weather from Open-Meteo (Real-time accurate data)
    if (weatherData.current) {
      const current = weatherData.current;
      
      // Temperature
      locationData.value.temp = Math.round(current.temperature_2m);
      
      // Humidity
      locationData.value.humidity = Math.round(current.relative_humidity_2m);
      
      // Weather Code
      locationData.value.weatherCode = current.weather_code;
      
      console.log(`Weather - Temp: ${locationData.value.temp}°C, Humidity: ${locationData.value.humidity}%, Code: ${locationData.value.weatherCode}`);
    } else {
      locationData.value.temp = '--';
      locationData.value.humidity = '--';
    }
    
    // 3. Get accurate location name via reverse geocoding
    try {
      const geoResponse = await fetch(`https://api.bigdatacloud.net/data/reverse-geocode-client?latitude=${lat}&longitude=${lng}&localityLanguage=en`);
      const geoData = await geoResponse.json();
      
      // Build location name from geocoding data
      const city = geoData.city || geoData.locality || geoData.localityInfo?.administrative?.[3]?.name;
      const state = geoData.principalSubdivision || geoData.localityInfo?.administrative?.[1]?.name;
      
      if (city && state) {
        locationData.value.name = `${city}, ${state}`;
      } else if (city) {
        locationData.value.name = city;
      } else if (waqiData.data?.city?.name) {
        // Fallback to WAQI station name if geocoding fails
        locationData.value.name = waqiData.data.city.name;
      } else {
        locationData.value.name = "Unknown Location";
      }
      
      console.log(`Location: ${locationData.value.name}`);
    } catch (error) {
      console.warn("Reverse geocoding failed:", error);
      // Use WAQI name as fallback
      if (waqiData.data?.city?.name) {
        locationData.value.name = waqiData.data.city.name;
      } else {
        locationData.value.name = "Location";
      }
    }
      
  } catch (error) {
    console.warn("Weather fetch error:", error);
    locationData.value.name = "Data Unavailable";
    locationData.value.outdoorAQI = 'N/A';
    locationData.value.temp = '--';
    locationData.value.humidity = '--';
  } finally {
    isLoadingData.value = false;
  }
};

const getWeatherIcon = (code) => {
  if (code === 0) return 'wb_sunny';
  if (code >= 1 && code <= 3) return 'partly_cloudy_day';
  if (code >= 45 && code <= 48) return 'fog';
  if (code >= 51 && code <= 67) return 'rainy';
  if (code >= 71 && code <= 77) return 'ac_unit';
  if (code >= 80 && code <= 82) return 'water_drop';
  if (code >= 95) return 'thunderstorm';
  return 'cloud';
};

// Helpers
const getAQIColor = (aqi) => {
  if (aqi <= 50) return 'green-4';
  if (aqi <= 100) return 'yellow-6';
  if (aqi <= 150) return 'orange-5';
  if (aqi <= 200) return 'red-5';
  return 'purple-4';
};

const getAQIStatus = (aqi) => {
  if (aqi <= 50) return 'Good';
  if (aqi <= 100) return 'Moderate';
  if (aqi <= 150) return 'Sensitive';
  if (aqi <= 200) return 'Unhealthy';
  return 'Hazardous';
};

const getAQIDescription = (aqi) => {
  if (aqi <= 50) return 'Air quality is satisfactory, and air pollution poses little or no risk.';
  if (aqi <= 100) return 'Air quality is acceptable. However, there may be a risk for some people.';
  if (aqi <= 150) return 'Members of sensitive groups may experience health effects. The general public is less likely to be affected.';
  if (aqi <= 200) return 'Everyone may begin to experience health effects; members of sensitive groups may experience more serious health effects.';
  return 'Health warnings of emergency conditions. The entire population is more likely to be affected.';
};

const getHealthAdvice = (aqi, temp, weatherCode) => {
  const warnings = [];
  
  // AQI Checks
  if (aqi > 150) warnings.push("🔴 High Pollution: Wear a mask outdoors. Avoid strenuous activity.");
  else if (aqi > 100) warnings.push("🟠 Unhealthy Air: Sensitive groups should reduce outdoor exertion.");
  
  // Temperature Checks
  if (temp > 35) warnings.push("🔥 Extreme Heat: Stay hydrated. Risk of heat exhaustion.");
  else if (temp < 5) warnings.push("❄️ Freezing: Dress warmly to avoid hypothermia.");
  
  // Weather Code Checks (Open-Meteo WMO codes)
  // Rain/Drizzle/Thunderstorm
  if ([51, 53, 55, 61, 63, 65, 80, 81, 82, 95, 96, 99].includes(weatherCode)) {
    warnings.push("☔ Wet Conditions: Slippery roads. Risk of viral infections increases.");
  }
  // Fog
  if ([45, 48].includes(weatherCode)) {
    warnings.push("🌫️ Low Visibility: Drive carefully. Respiratory irritation possible.");
  }

  if (warnings.length === 0) return "✅ Conditions are good. Enjoy the outdoors!";
  return warnings.join(" ");
};

const getBPMStatus = (bpm) => {
  if (bpm < 60) return { text: 'Low Resting Rate', color: 'text-blue-4' };
  if (bpm <= 100) return { text: 'Normal Range', color: 'text-green-4' };
  return { text: 'Elevated Rate', color: 'text-orange-4' };
};

const getAlertClass = (aqi, temp) => {
  if (aqi > 150 || temp > 35 || temp < 5) return 'bg-red-dim text-red-2';
  if (aqi > 100) return 'bg-orange-dim text-orange-2';
  return 'bg-green-dim text-green-2';
};

const getSpO2Status = (spo2) => {
  if (spo2 >= 95) return { text: 'Normal', color: 'text-green-4' };
  if (spo2 >= 90) return { text: 'Borderline', color: 'text-yellow-4' };
  return { text: 'Low - Hypoxia Risk', color: 'text-red-4' };
};

// History Filtering & Formatting
const filteredHistory = computed(() => {
  let filtered = historyList.value;
  
  // Filter by date
  if (dateFilter.value) {
    filtered = filtered.filter(item => {
      if (!item.timestamp) return false;
      const itemDate = new Date(item.timestamp);
      const filterDate = new Date(dateFilter.value);
      return itemDate.toDateString() === filterDate.toDateString();
    });
  }
  
  // Filter by time range
  if (timeFilterStart.value || timeFilterEnd.value) {
    filtered = filtered.filter(item => {
      if (!item.timestamp) return false;
      const itemDate = new Date(item.timestamp);
      const itemTime = itemDate.getHours() * 60 + itemDate.getMinutes();
      
      if (timeFilterStart.value) {
        const [startH, startM] = timeFilterStart.value.split(':');
        const startTime = parseInt(startH) * 60 + parseInt(startM);
        if (itemTime < startTime) return false;
      }
      
      if (timeFilterEnd.value) {
        const [endH, endM] = timeFilterEnd.value.split(':');
        const endTime = parseInt(endH) * 60 + parseInt(endM);
        if (itemTime > endTime) return false;
      }
      
      return true;
    });
  }
  
  return filtered;
});

const formatDate = (timestamp) => {
  if (!timestamp) return 'N/A';
  const date = new Date(timestamp);
  return date.toLocaleDateString('en-IN', { 
    year: 'numeric', 
    month: 'short', 
    day: 'numeric' 
  });
};

const formatTime = (timestamp) => {
  if (!timestamp) return 'N/A';
  const date = new Date(timestamp);
  return date.toLocaleTimeString('en-IN', { 
    hour: '2-digit', 
    minute: '2-digit',
    second: '2-digit'
  });
};
</script>

<style scoped>
/* Base Layout */
.iot-dashboard {
  min-height: 100vh;
  font-family: 'Inter', 'Roboto', sans-serif;
  color: #fff;
  overflow-x: hidden;
}

.tracking-wider {
  letter-spacing: 2px;
}

.opacity-80 {
  opacity: 0.8;
}

/* Updated Background - Professional Dark Slate/Blue */
.gradient-background {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: radial-gradient(circle at 10% 20%, rgb(30, 35, 50) 0%, rgb(15, 18, 26) 90%);
  z-index: -1;
}

.content-wrapper {
  position: relative;
  z-index: 1;
}

/* Glass Badge */
.glass-badge {
  background: rgba(255, 255, 255, 0.1);
  backdrop-filter: blur(8px);
  border: 1px solid rgba(255, 255, 255, 0.1);
}

/* Test Mode Banner */
.test-mode-banner {
  background: linear-gradient(135deg, rgba(156, 39, 176, 0.2) 0%, rgba(103, 58, 183, 0.2) 100%);
  backdrop-filter: blur(10px);
  border: 1px solid rgba(156, 39, 176, 0.3);
  border-radius: 12px;
  padding: 12px 16px;
  animation: pulse-border 2s ease-in-out infinite;
}

@keyframes pulse-border {
  0%, 100% {
    border-color: rgba(156, 39, 176, 0.3);
    box-shadow: 0 0 0 0 rgba(156, 39, 176, 0.4);
  }
  50% {
    border-color: rgba(156, 39, 176, 0.6);
    box-shadow: 0 0 0 4px rgba(156, 39, 176, 0.1);
  }
}

/* Professional Card Design */
.pro-card {
  background: rgba(255, 255, 255, 0.03);
  backdrop-filter: blur(16px);
  -webkit-backdrop-filter: blur(16px);
  border-radius: 16px;
  border: 1px solid rgba(255, 255, 255, 0.08);
  padding: 16px; /* Compact padding */
  box-shadow: 0 4px 24px -1px rgba(0, 0, 0, 0.2);
  transition: all 0.4s cubic-bezier(0.175, 0.885, 0.32, 1.275);
  height: 100%;
}

.pro-card:hover {
  transform: translateY(-2px); /* Reduced hover movement */
  background: rgba(255, 255, 255, 0.05);
  box-shadow: 0 12px 32px -1px rgba(0, 0, 0, 0.3);
  border-color: rgba(255, 255, 255, 0.15);
}

/* Card Internals */
.card-top {
  display: flex;
  align-items: center;
  margin-bottom: 8px; /* Reduced margin */
}

.icon-box {
  width: 32px;
  height: 32px;
  border-radius: 8px;
  display: flex;
  align-items: center;
  justify-content: center;
  margin-right: 8px;
  font-size: 16px;
}

.bg-red-grad { background: linear-gradient(135deg, #FF6B6B 0%, #EE5253 100%); box-shadow: 0 4px 12px rgba(238, 82, 83, 0.4); }
.bg-blue-grad { background: linear-gradient(135deg, #48DBFB 0%, #0ABDE3 100%); box-shadow: 0 4px 12px rgba(10, 189, 227, 0.4); }

.metric-value {
  font-size: 2.5rem; /* Reduced from 4rem */
  font-weight: 200;
  line-height: 1;
  letter-spacing: -1px;
  margin-bottom: 8px;
}

.metric-unit {
  font-size: 0.9rem;
  font-weight: 500;
  opacity: 0.6;
  letter-spacing: 0;
  margin-left: 2px;
}

.card-footer {
  margin-top: 10px;
  padding-top: 15px;
  border-top: 1px solid rgba(255, 255, 255, 0.05);
  font-size: 0.9rem;
}

/* Visualizers */
.visualizer {
  height: 40px;
  display: flex;
  align-items: center;
  overflow: hidden;
  position: relative;
}

.heart-dot {
  width: 8px;
  height: 8px;
  background-color: #EE5253;
  border-radius: 50%;
  animation: pulse-dot 1s cubic-bezier(0.4, 0, 0.6, 1) infinite;
  box-shadow: 0 0 10px #EE5253;
}

@keyframes pulse-dot {
  0% { transform: scale(0.95); opacity: 0.7; }
  50% { transform: scale(1.3); opacity: 1; }
  100% { transform: scale(0.95); opacity: 0.7; }
}

.progress-bar-container {
  height: 6px;
  background: rgba(255,255,255,0.1);
  border-radius: 3px;
  width: 100%;
  overflow: hidden;
}

.progress-bar {
  height: 100%;
  background: linear-gradient(90deg, #48DBFB, #0ABDE3);
  border-radius: 3px;
  transition: width 1s ease;
}

/* Alerts */
.alert-banner {
  background: rgba(220, 20, 60, 0.1);
  border: 1px solid rgba(220, 20, 60, 0.5);
  backdrop-filter: blur(10px);
  border-radius: 16px;
  padding: 20px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  animation: bg-pulse 2s infinite;
}

.alert-content {
  display: flex;
  align-items: center;
}

.alert-icon-wrapper {
  background: rgba(220, 20, 60, 0.2);
  width: 48px;
  height: 48px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  margin-right: 16px;
}

.alert-icon {
  font-size: 24px;
  color: #ff4444;
  animation: shake 0.5s ease-in-out infinite;
}

@keyframes bg-pulse {
  0% { box-shadow: 0 0 0 0 rgba(220, 20, 60, 0.4); border-color: rgba(220, 20, 60, 0.5); }
  70% { box-shadow: 0 0 0 10px rgba(220, 20, 60, 0); border-color: rgba(220, 20, 60, 0.8); }
  100% { box-shadow: 0 0 0 0 rgba(220, 20, 60, 0); border-color: rgba(220, 20, 60, 0.5); }
}

/* Utilities */
.border-right {
  border-right: 1px solid rgba(255,255,255,0.05);
}

.lh-loose {
  line-height: 1.6;
}

@media (max-width: 600px) {
  .metric-value { font-size: 3rem; }
  .border-right { border-right: none; border-bottom: 1px solid rgba(255,255,255,0.05); }
}

.slide-fade-enter-active, .slide-fade-leave-active { transition: all 0.5s ease; }
.slide-fade-enter-from, .slide-fade-leave-to { transform: translateY(-20px); opacity: 0; }

/* Glass Dialog Styling */
.glass-dialog {
  background: rgba(30, 35, 50, 0.95);
  backdrop-filter: blur(16px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: 16px;
  color: white;
}

.weather-metrics {
  background: rgba(255, 255, 255, 0.05);
  border-radius: 12px;
  padding: 12px;
}

/* Dimmed Backgrounds for Alerts */
.bg-red-dim { background: rgba(244, 67, 54, 0.2); border: 1px solid rgba(244, 67, 54, 0.3); }
.bg-orange-dim { background: rgba(255, 152, 0, 0.2); border: 1px solid rgba(255, 152, 0, 0.3); }
.bg-green-dim { background: rgba(76, 175, 80, 0.2); border: 1px solid rgba(76, 175, 80, 0.3); }

/* ========== MODERN UI/UX ENHANCEMENTS ========== */

/* Gradient Borders for Cards */
.pro-card {
  position: relative;
  overflow: hidden;
}

.pro-card::before {
  content: '';
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  border-radius: 16px;
  padding: 1px;
  background: linear-gradient(135deg, 
    rgba(102, 126, 234, 0.3) 0%, 
    rgba(118, 75, 162, 0.3) 50%,
    rgba(102, 126, 234, 0.3) 100%);
  -webkit-mask: linear-gradient(#fff 0 0) content-box, linear-gradient(#fff 0 0);
  -webkit-mask-composite: xor;
  mask: linear-gradient(#fff 0 0) content-box, linear-gradient(#fff 0 0);
  mask-composite: exclude;
  opacity: 0;
  transition: opacity 0.3s ease;
  pointer-events: none;
}

.pro-card:hover::before {
  opacity: 1;
}

/* Animated Number Count-up */
@keyframes number-pop {
  0% { transform: scale(1); }
  50% { transform: scale(1.1); }
  100% { transform: scale(1); }
}

.metric-value {
  animation: number-pop 0.3s ease-out;
}

/* Shimmer Loading Effect */
@keyframes shimmer {
  0% { background-position: -1000px 0; }
  100% { background-position: 1000px 0; }
}

.q-skeleton {
  background: linear-gradient(
    90deg,
    rgba(255, 255, 255, 0.05) 0%,
    rgba(255, 255, 255, 0.15) 50%,
    rgba(255, 255, 255, 0.05) 100%
  );
  background-size: 1000px 100%;
  animation: shimmer 2s infinite;
}

/* Pulsing Status Indicator */
@keyframes pulse-dot {
  0%, 100% {
    opacity: 1;
    transform: scale(1);
  }
  50% {
    opacity: 0.7;
    transform: scale(1.2);
  }
}

.status-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  display: inline-block;
  animation: pulse-dot 2s ease-in-out infinite;
}

.status-dot.online { background: #4caf50; box-shadow: 0 0 8px #4caf50; }
.status-dot.offline { background: #f44336; box-shadow: 0 0 8px #f44336; }

/* Smooth Transitions */
* {
  transition: background-color 0.3s ease, 
              border-color 0.3s ease,
              transform 0.3s cubic-bezier(0.175, 0.885, 0.32, 1.275);
}

/* Touch-Friendly Buttons */
.q-btn {
  min-height: 44px;
  min-width: 44px;
  touch-action: manipulation;
}

/* Enhanced Ripple Effect */
.q-btn:active {
  transform: scale(0.95);
}

/* Slide Transitions */
.slide-down-enter-active,
.slide-down-leave-active {
  transition: all 0.3s cubic-bezier(0.25, 0.8, 0.25, 1);
}

.slide-down-enter-from {
  opacity: 0;
  transform: translateY(-20px);
}

.slide-down-leave-to {
  opacity: 0;
  transform: translateY(-10px);
}

/* Mobile Optimizations */
@media (max-width: 599px) {
  /* Larger Touch Targets */
  .q-btn {
    min-height: 48px;
    min-width: 48px;
  }
  
  /* Increased Font Sizes */
  .text-h3 { font-size: 2rem !important; }
  .text-h4 { font-size: 1.75rem !important; }
  .text-h5 { font-size: 1.5rem !important; }
  .text-h6 { font-size: 1.25rem !important; }
  
  /* Better Card Spacing */
  .pro-card {
    padding: 20px !important;
    margin-bottom: 16px;
  }
  
  /* Responsive Metrics */
  .metric-value {
    font-size: 3rem !important;
  }
  
  /* Full-Width Dialogs */
  .q-dialog__inner {
    padding: 8px;
  }
  
  /* Sticky Header */
  .dashboard-title {
    position: sticky;
    top: 0;
    z-index: 100;
    background: rgba(26, 32, 44, 0.95);
    backdrop-filter: blur(10px);
    padding: 16px 0;
    margin: -16px 0 16px 0;
  }
}

/* Tablet Optimizations */
@media (min-width: 600px) and (max-width: 1023px) {
  .pro-card {
    padding: 18px !important;
  }
  
  .metric-value {
    font-size: 2.75rem !important;
  }
}

/* Enhanced Scrollbar */
::-webkit-scrollbar {
  width: 8px;
  height: 8px;
}

::-webkit-scrollbar-track {
  background: rgba(255, 255, 255, 0.05);
  border-radius: 4px;
}

::-webkit-scrollbar-thumb {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  border-radius: 4px;
}

::-webkit-scrollbar-thumb:hover {
  background: linear-gradient(135deg, #764ba2 0%, #667eea 100%);
}

/* Glow Effects */
.glow-on-hover {
  transition: all 0.3s ease;
}

.glow-on-hover:hover {
  box-shadow: 0 0 20px rgba(102, 126, 234, 0.5),
              0 0 40px rgba(118, 75, 162, 0.3);
}

/* Status Badge Animations */
.q-badge {
  animation: badge-appear 0.3s cubic-bezier(0.175, 0.885, 0.32, 1.275);
}

@keyframes badge-appear {
  0% {
    opacity: 0;
    transform: scale(0.5);
  }
  100% {
    opacity: 1;
    transform: scale(1);
  }
}

/* Icon Rotation on Hover */
.q-icon {
  transition: transform 0.3s ease;
}

.q-btn:hover .q-icon {
  transform: rotate(15deg) scale(1.1);
}

/* Card Lift Effect */
.pro-card {
  will-change: transform;
}

.pro-card:active {
  transform: translateY(0) scale(0.98);
}

/* Smooth Focus States */
.q-field--focused .q-field__control {
  box-shadow: 0 0 0 2px rgba(102, 126, 234, 0.5);
}

/* Enhanced Toggle Switches */
.q-toggle {
  transform-origin: center;
}

.q-toggle:active {
  transform: scale(0.95);
}

/* Gradient Text */
.gradient-text {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
  background-clip: text;
}

/* Floating Action Button */
.fab {
  position: fixed;
  bottom: 24px;
  right: 24px;
  z-index: 1000;
  box-shadow: 0 8px 24px rgba(102, 126, 234, 0.4);
}

@media (max-width: 599px) {
  .fab {
    bottom: 80px; /* Above mobile navigation */
  }
}

/* Skeleton Pulse */
@keyframes skeleton-pulse {
  0%, 100% { opacity: 1; }
  50% { opacity: 0.4; }
}

.q-skeleton {
  animation: skeleton-pulse 1.5s ease-in-out infinite;
}

/* Performance Optimizations */
.pro-card,
.q-btn,
.q-icon {
  will-change: transform;
  backface-visibility: hidden;
  -webkit-font-smoothing: antialiased;
}

/* Accessibility - High Contrast Focus */
*:focus-visible {
  outline: 2px solid #667eea;
  outline-offset: 2px;
}

/* Print Styles */
@media print {
  .gradient-background,
  .q-btn,
  .fab {
    display: none !important;
  }
  
  .pro-card {
    break-inside: avoid;
    border: 1px solid #333;
  }
}
</style>