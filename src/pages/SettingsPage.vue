<template>
  <q-page class="settings-page">
    <div class="gradient-background"></div>
    
    <div class="q-pa-md content-wrapper">
      <div class="text-h4 text-white text-center q-mb-lg">
        <q-icon name="settings" size="32px" class="q-mr-sm" />
        Settings
      </div>

      <div class="row justify-center">
        <div class="col-12 col-md-8">
          
          <!-- Emergency Contacts Section -->
          <div class="glass-card q-mb-md">
            <div class="card-header q-mb-md">
              <q-icon name="contact_phone" size="28px" color="red-6" />
              <span class="text-h6 q-ml-sm text-white">Emergency Contacts</span>
            </div>

            <!-- Contact List -->
            <div v-if="hasContacts" class="q-mb-md">
              <q-list dark separator>
                <q-item v-for="contact in contacts" :key="contact.id" class="contact-item">
                  <q-item-section avatar>
                    <q-avatar :color="contact.priority === 'primary' ? 'red-6' : 'blue-6'" text-color="white">
                      <q-icon name="person" />
                    </q-avatar>
                  </q-item-section>

                  <q-item-section>
                    <q-item-label class="text-white text-weight-bold">
                      {{ contact.name }}
                      <q-badge v-if="contact.priority === 'primary'" color="red" class="q-ml-sm">PRIMARY</q-badge>
                    </q-item-label>
                    <q-item-label caption class="text-grey-4">
                      {{ contact.phone }} • {{ contact.relationship }}
                    </q-item-label>
                  </q-item-section>

                  <q-item-section side>
                    <div class="row q-gutter-xs">
                      <q-btn flat dense round icon="call" color="green" size="sm" @click="testCall(contact)">
                        <q-tooltip>Test Call</q-tooltip>
                      </q-btn>
                      <q-btn flat dense round icon="message" color="blue" size="sm" @click="testSMS(contact)">
                        <q-tooltip>Test SMS</q-tooltip>
                      </q-btn>
                      <q-btn flat dense round icon="edit" color="orange" size="sm" @click="editContact(contact)">
                        <q-tooltip>Edit</q-tooltip>
                      </q-btn>
                      <q-btn flat dense round icon="delete" color="red" size="sm" @click="confirmDelete(contact)">
                        <q-tooltip>Delete</q-tooltip>
                      </q-btn>
                    </div>
                  </q-item-section>
                </q-item>
              </q-list>
            </div>

            <div v-else class="text-center q-pa-md">
              <q-icon name="person_add" size="64px" color="grey-6" />
              <div class="text-grey-5 q-mt-sm">No emergency contacts added yet</div>
            </div>

            <!-- Add Contact Button -->
            <q-btn 
              color="primary" 
              icon="add" 
              label="Add Emergency Contact" 
              class="full-width"
              @click="showAddDialog = true"
            />
          </div>

          <!-- Emergency Settings -->
          <div class="glass-card q-mb-md">
            <div class="card-header q-mb-md">
              <q-icon name="tune" size="28px" color="purple-6" />
              <span class="text-h6 q-ml-sm text-white">Emergency Settings</span>
            </div>

            <div class="q-gutter-md">
              <q-toggle
                v-model="settings.autoCall"
                @update:model-value="saveSettings"
                label="Auto-call primary contact on fall detection"
                color="green"
                dark
              />

              <q-toggle
                v-model="settings.autoSMS"
                @update:model-value="saveSettings"
                label="Auto-send SMS to all contacts"
                color="blue"
                dark
              />

              <q-toggle
                v-model="settings.requireConfirmation"
                @update:model-value="saveSettings"
                label="Require confirmation before calling"
                color="orange"
                dark
              />

              <q-input
                v-model="settings.customMessage"
                @blur="saveSettings"
                label="Custom Emergency Message"
                type="textarea"
                rows="3"
                dark
                filled
                hint="Leave blank to use default message"
              />
            </div>
          </div>

          <!-- Back Button -->
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
import { ref } from 'vue';
import { useEmergencyContacts } from '../composables/useEmergencyContacts';
import { useQuasar } from 'quasar';

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
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(10px);
  border-radius: 16px;
  border: 1px solid rgba(255, 255, 255, 0.1);
  padding: 24px;
  box-shadow: 0 8px 32px 0 rgba(31, 38, 135, 0.37);
}

.card-header {
  display: flex;
  align-items: center;
  padding-bottom: 16px;
  border-bottom: 1px solid rgba(255, 255, 255, 0.1);
}

.contact-item {
  background: rgba(255, 255, 255, 0.03);
  border-radius: 8px;
  margin-bottom: 8px;
}

.glass-btn {
  backdrop-filter: blur(5px);
  background: rgba(255, 255, 255, 0.9) !important;
}
</style>
