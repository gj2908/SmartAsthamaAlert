<template>
  <q-page class="settings-page">
    <div class="gradient-background"></div>
    
    <div class="q-pa-md content-wrapper">
      <div class="text-h4 text-white text-center q-mb-lg">
        <q-icon name="settings" size="32px" class="q-mr-sm" />
        Settings
      </div>

      <div class="row q-col-gutter-lg justify-center">
        <div class="col-12 col-md-6">
          <!-- Emergency Contacts Section -->
          <div class="glass-card q-mb-lg full-height">
            <div class="card-header q-mb-md">
              <q-icon name="contact_phone" size="28px" color="red-6" />
              <span class="text-subtitle1 q-ml-sm text-white text-weight-bold">Emergency Contacts</span>
            </div>

            <!-- Contact List -->
            <div v-if="hasContacts" class="q-mb-md">
              <q-list dark separator>
                <q-item v-for="contact in contacts" :key="contact.id" class="contact-item">
                  <q-item-section avatar>
                    <q-avatar :color="contact.priority === 'primary' ? 'red-6' : 'blue-6'" text-color="white" size="32px">
                      <q-icon name="person" size="18px" />
                    </q-avatar>
                  </q-item-section>

                  <q-item-section>
                    <q-item-label class="text-white text-weight-medium">
                      {{ contact.name }}
                      <q-badge v-if="contact.priority === 'primary'" color="red-9" size="xs" class="q-ml-sm">PRIME</q-badge>
                    </q-item-label>
                    <q-item-label caption class="text-grey-5" style="font-size: 0.75rem">
                      {{ contact.phone }}
                    </q-item-label>
                  </q-item-section>

                  <q-item-section side>
                    <div class="row q-gutter-xs">
                      <q-btn flat dense round icon="call" color="green-4" size="sm" @click="testCall(contact)" />
                      <q-btn flat dense round icon="message" color="blue-4" size="sm" @click="testSMS(contact)" />
                      <q-btn flat dense round icon="edit" color="orange-4" size="sm" @click="editContact(contact)" />
                      <q-btn flat dense round icon="delete" color="red-4" size="sm" @click="confirmDelete(contact)" />
                    </div>
                  </q-item-section>
                </q-item>
              </q-list>
            </div>

            <div v-else class="text-center q-pa-lg">
              <q-icon name="person_add" size="48px" color="grey-7" />
              <div class="text-grey-6 q-mt-sm">No emergency contacts</div>
            </div>

            <q-btn outline color="primary" icon="add" label="Add New Contact" class="full-width q-mt-md" @click="showAddDialog = true" />
          </div>
        </div>

        <div class="col-12 col-md-6">
          <!-- Emergency Settings -->
          <div class="glass-card q-mb-lg">
            <div class="card-header q-mb-md">
              <q-icon name="tune" size="28px" color="purple-6" />
              <span class="text-subtitle1 q-ml-sm text-white text-weight-bold">Response Protocol</span>
            </div>

            <div class="q-gutter-sm">
              <q-toggle v-model="settings.autoCall" @update:model-value="saveSettings" label="Auto-call on fall detection" color="green-4" dark dense />
              <q-toggle v-model="settings.autoSMS" @update:model-value="saveSettings" label="Auto-send SMS alert" color="blue-4" dark dense />
              <q-toggle v-model="settings.requireConfirmation" @update:model-value="saveSettings" label="Pre-call confirmation" color="orange-4" dark dense />
              
              <q-input v-model="settings.customMessage" @blur="saveSettings" label="Custom SOS Message" type="textarea" rows="2" dark filled dense class="q-mt-sm" />
            </div>
          </div>

          <!-- Hardware Diagnostics -->
          <div class="glass-card">
            <div class="card-header q-mb-md">
              <q-icon name="developer_board" size="28px" color="blue-6" />
              <span class="text-subtitle1 q-ml-sm text-white text-weight-bold">Hardware & System</span>
              <q-space />
              <q-badge :color="isConnected ? 'green-9' : 'red-9'" rounded class="q-px-sm">
                {{ isConnected ? 'ONLINE' : 'OFFLINE' }}
              </q-badge>
            </div>

            <div class="row q-col-gutter-sm q-mb-md">
              <div class="col-4">
                <div class="diag-item q-pa-sm rounded-borders bg-dark-glass text-center">
                  <div class="text-caption text-grey-5" style="font-size: 0.65rem">Internal IP</div>
                  <div class="text-body2 text-white text-weight-bold" style="font-size: 0.75rem">{{ deviceInfo.ip }}</div>
                </div>
              </div>
              <div class="col-4">
                <div class="diag-item q-pa-sm rounded-borders bg-dark-glass text-center">
                  <div class="text-caption text-grey-5" style="font-size: 0.65rem">Public IP</div>
                  <div class="text-body2 text-blue-4 text-weight-bold" style="font-size: 0.75rem">{{ publicIP }}</div>
                </div>
              </div>
              <div class="col-4">
                <div class="diag-item q-pa-sm rounded-borders bg-dark-glass text-center">
                  <div class="text-caption text-grey-5" style="font-size: 0.65rem">Sync</div>
                  <div class="text-body2 text-white text-weight-bold" style="font-size: 0.75rem">{{ deviceInfo.lastSync }}</div>
                </div>
              </div>
            </div>

            <div class="row q-gutter-sm q-mb-md">
              <q-btn outline color="orange-5" dense icon="refresh" label="Restart Device" class="col" @click="sendDeviceCommand('restart')" />
              <q-btn outline color="blue-5" dense icon="straighten" label="Calibrate Sensors" class="col" @click="sendDeviceCommand('recalibrate')" />
            </div>

            <q-expansion-item
              dark
              dense
              icon="history"
              label="Recent Fall Logs"
              header-class="text-grey-4"
              class="bg-dark-glass rounded-borders"
            >
              <div class="q-pa-sm">
                <q-list dark dense separator v-if="fallRecords.length > 0">
                  <q-item v-for="record in fallRecords" :key="record.id" dense class="q-px-none">
                    <q-item-section>
                      <q-item-label class="text-caption text-grey-3">
                        {{ new Date(record.timestamp).toLocaleString() }}
                      </q-item-label>
                    </q-item-section>
                    <q-item-section side>
                      <q-badge color="red-9" label="FALL" size="xs" />
                    </q-item-section>
                  </q-item>
                </q-list>
                <div v-else class="text-center text-grey-6 italic text-caption">No incidents recorded</div>
              </div>
            </q-expansion-item>
          </div>
        </div>
      </div>

      <div class="text-center q-mt-xl">
        <q-btn to="/" flat color="grey-5" label="Return to Dashboard" icon="arrow_back" />
      </div>
    </div>

    <!-- Add/Edit Contact Dialog -->
    <q-dialog v-model="showAddDialog" persistent>
      <q-card dark class="bg-grey-9" style="min-width: 350px">
        <q-card-section>
          <div class="text-h6">{{ editingContact ? 'Edit Contact' : 'Add Emergency Contact' }}</div>
        </q-card-section>

        <q-card-section class="q-pt-none">
          <q-input
            v-model="contactForm.name"
            label="Name *"
            dark
            filled
            class="q-mb-md"
            :rules="[val => !!val || 'Name is required']"
          />

          <q-input
            v-model="contactForm.phone"
            label="Phone Number *"
            dark
            filled
            class="q-mb-md"
            mask="(###) ###-####"
            hint="10-digit phone number"
            :rules="[val => validatePhone(val) || 'Invalid phone number']"
          />

          <q-input
            v-model="contactForm.relationship"
            label="Relationship"
            dark
            filled
            class="q-mb-md"
            placeholder="e.g., Family, Doctor, Friend"
          />

          <q-select
            v-model="contactForm.priority"
            :options="priorityOptions"
            label="Priority"
            dark
            filled
            emit-value
            map-options
          />
        </q-card-section>

        <q-card-actions align="right">
          <q-btn flat label="Cancel" color="grey" v-close-popup @click="resetForm" />
          <q-btn flat label="Save" color="primary" @click="saveContact" />
        </q-card-actions>
      </q-card>
    </q-dialog>
  </q-page>
</template>

<script setup>
import { ref, onMounted } from 'vue';
import { useEmergencyContacts } from '../composables/useEmergencyContacts';
import { useQuasar } from 'quasar';
import { initializeApp, getApps, getApp } from 'firebase/app';
import { getDatabase, ref as dbRef, onValue, query, limitToLast } from 'firebase/database';

const $q = useQuasar();

const {
  contacts,
  settings,
  hasContacts,
  addContact,
  updateContact,
  deleteContact,
  updateSettings: saveSettingsToStore,
  validatePhone,
  makeEmergencyCall,
  sendEmergencySMS
} = useEmergencyContacts();

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

// Device States
const isConnected = ref(false);
const deviceInfo = ref({ ip: '--', lastSync: '--' });
const publicIP = ref('Detecting...');
const fallRecords = ref([]);

let database = null;

onMounted(() => {
  try {
    const app = !getApps().length ? initializeApp(firebaseConfig) : getApp();
    database = getDatabase(app);
    
    // Listen for device info
    onValue(dbRef(database, 'sensor_data'), (snapshot) => {
      const data = snapshot.val();
      if (data) {
        isConnected.value = true;
        if (data.device_ip) deviceInfo.value.ip = data.device_ip;
        deviceInfo.value.lastSync = new Date().toLocaleTimeString();
      }
    });

    // Listen for Fall Records
    const fallRecRef = query(dbRef(database, 'fall_records'), limitToLast(5));
    onValue(fallRecRef, (snapshot) => {
      const data = snapshot.val();
      if (data) {
        fallRecords.value = Object.keys(data).map(key => ({
          id: key,
          ...data[key]
        })).reverse();
      }
    });
    
    // Fetch Public IP
    fetch('https://api.ipify.org?format=json')
      .then(r => r.json())
      .then(data => publicIP.value = data.ip)
      .catch(() => publicIP.value = 'Failed');

  } catch (err) {
    console.error('Firebase error in Settings:', err);
  }
});

const sendDeviceCommand = async (command) => {
  console.log(`Sending command to device: ${command}`);
  try {
    // Reverting to manual REST PUT to ensure 100% compatibility with Arduino's manual HTTP parser
    const url = `${firebaseConfig.databaseURL}/sensor_data/commands.json`;
    const response = await fetch(url, {
      method: "PUT",
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(command)
    });
    
    if (response.ok) {
      $q.notify({
        type: 'positive',
        message: `Command sent: ${command.toUpperCase()}`,
        caption: 'Device should respond in < 5s',
        position: 'top',
        icon: 'send'
      });
    } else {
      throw new Error(`HTTP Error: ${response.status}`);
    }
  } catch (error) {
    console.error('Command failed:', error);
    $q.notify({
      type: 'negative',
      message: 'Failed to send command. Check connection.',
      position: 'top',
      icon: 'error'
    });
  }
};

const showAddDialog = ref(false);
const editingContact = ref(null);
const contactForm = ref({
  name: '',
  phone: '',
  relationship: '',
  priority: 'secondary'
});

const priorityOptions = [
  { label: 'Primary Contact', value: 'primary' },
  { label: 'Secondary Contact', value: 'secondary' },
  { label: 'Tertiary Contact', value: 'tertiary' }
];

const saveSettings = () => {
  saveSettingsToStore(settings.value);
  $q.notify({
    type: 'positive',
    message: 'Settings saved',
    position: 'top',
    timeout: 2000
  });
};

const saveContact = () => {
  try {
    if (!contactForm.value.name || !contactForm.value.phone) {
      $q.notify({
        type: 'negative',
        message: 'Please fill in all required fields',
        position: 'top'
      });
      return;
    }

    if (!validatePhone(contactForm.value.phone)) {
      $q.notify({
        type: 'negative',
        message: 'Invalid phone number',
        position: 'top'
      });
      return;
    }

    if (editingContact.value) {
      updateContact(editingContact.value.id, contactForm.value);
      $q.notify({
        type: 'positive',
        message: 'Contact updated',
        position: 'top'
      });
    } else {
      addContact(contactForm.value);
      $q.notify({
        type: 'positive',
        message: 'Contact added',
        position: 'top'
      });
    }

    showAddDialog.value = false;
    resetForm();
  } catch (error) {
    $q.notify({
      type: 'negative',
      message: error.message,
      position: 'top'
    });
  }
};

const editContact = (contact) => {
  editingContact.value = contact;
  contactForm.value = { ...contact };
  showAddDialog.value = true;
};

const confirmDelete = (contact) => {
  $q.dialog({
    title: 'Confirm Delete',
    message: `Delete ${contact.name} from emergency contacts?`,
    cancel: true,
    persistent: true,
    dark: true
  }).onOk(() => {
    deleteContact(contact.id);
    $q.notify({
      type: 'positive',
      message: 'Contact deleted',
      position: 'top'
    });
  });
};

const resetForm = () => {
  contactForm.value = {
    name: '',
    phone: '',
    relationship: '',
    priority: 'secondary'
  };
  editingContact.value = null;
};

const testCall = (contact) => {
  makeEmergencyCall(contact);
  $q.notify({
    type: 'info',
    message: `Calling ${contact.name}...`,
    position: 'top',
    timeout: 2000
  });
};

const testSMS = (contact) => {
  const testMessage = `This is a test message from Safety Monitor. Your contact is configured correctly!`;
  sendEmergencySMS(contact, testMessage);
  $q.notify({
    type: 'info',
    message: `Opening SMS to ${contact.name}...`,
    position: 'top',
    timeout: 2000
  });
};
</script>

<style scoped>
.settings-page {
  position: relative;
  min-height: 100vh;
}

.gradient-background {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: linear-gradient(135deg, #1a1a2e 0%, #16213e 50%, #0f3460 100%);
  z-index: -1;
}

.content-wrapper {
  position: relative;
  z-index: 1;
}

.glass-card {
  background: rgba(255, 255, 255, 0.03);
  backdrop-filter: blur(15px);
  border-radius: 20px;
  border: 1px solid rgba(255, 255, 255, 0.08);
  padding: 20px;
  box-shadow: 0 12px 40px rgba(0, 0, 0, 0.4);
}

.card-header {
  display: flex;
  align-items: center;
  padding-bottom: 12px;
  border-bottom: 1px solid rgba(255, 255, 255, 0.05);
}

.contact-item {
  background: rgba(255, 255, 255, 0.02);
  border-radius: 12px;
  margin-bottom: 8px;
  padding: 8px;
}

.bg-dark-glass {
  background: rgba(0, 0, 0, 0.2);
}

.min-width-auto { min-width: auto !important; }
.min-height-auto { min-height: auto !important; }
</style>
