import { ref, computed } from 'vue';

// Emergency contact structure
const STORAGE_KEY = 'emergency_contacts';
const SETTINGS_KEY = 'emergency_settings';

// Reactive state
const contacts = ref([]);
const settings = ref({
    autoCall: true,
    autoSMS: true,
    requireConfirmation: false,
    customMessage: ''
});

// Load from localStorage
const loadContacts = () => {
    try {
        const stored = localStorage.getItem(STORAGE_KEY);
        if (stored) {
            contacts.value = JSON.parse(stored);
        }
    } catch (error) {
        console.error('Error loading contacts:', error);
    }
};

const loadSettings = () => {
    try {
        const stored = localStorage.getItem(SETTINGS_KEY);
        if (stored) {
            settings.value = { ...settings.value, ...JSON.parse(stored) };
        }
    } catch (error) {
        console.error('Error loading settings:', error);
    }
};

// Save to localStorage
const saveContacts = () => {
    try {
        localStorage.setItem(STORAGE_KEY, JSON.stringify(contacts.value));
    } catch (error) {
        console.error('Error saving contacts:', error);
    }
};

const saveSettings = () => {
    try {
        localStorage.setItem(SETTINGS_KEY, JSON.stringify(settings.value));
    } catch (error) {
        console.error('Error saving settings:', error);
    }
};

// Validate phone number
const validatePhone = (phone) => {
    // Remove all non-numeric characters
    const cleaned = phone.replace(/\D/g, '');
    // Check if it's a valid length (10 digits for most countries)
    return cleaned.length >= 10 && cleaned.length <= 15;
};

// Format phone number
const formatPhone = (phone) => {
    const cleaned = phone.replace(/\D/g, '');
    return cleaned;
};

export function useEmergencyContacts() {
    // Initialize
    if (contacts.value.length === 0) {
        loadContacts();
    }
    if (!settings.value.customMessage) {
        loadSettings();
    }

    // Computed
    const primaryContact = computed(() =>
        contacts.value.find(c => c.priority === 'primary') || contacts.value[0]
    );

    const hasContacts = computed(() => contacts.value.length > 0);

    // Methods
    const addContact = (contact) => {
        if (!validatePhone(contact.phone)) {
            throw new Error('Invalid phone number');
        }

        const newContact = {
            id: Date.now(),
            name: contact.name,
            phone: formatPhone(contact.phone),
            relationship: contact.relationship || 'Emergency Contact',
            priority: contact.priority || 'secondary',
            createdAt: new Date().toISOString()
        };

        contacts.value.push(newContact);
        saveContacts();
        return newContact;
    };

    const updateContact = (id, updates) => {
        const index = contacts.value.findIndex(c => c.id === id);
        if (index !== -1) {
            if (updates.phone && !validatePhone(updates.phone)) {
                throw new Error('Invalid phone number');
            }

            contacts.value[index] = {
                ...contacts.value[index],
                ...updates,
                phone: updates.phone ? formatPhone(updates.phone) : contacts.value[index].phone
            };
            saveContacts();
        }
    };

    const deleteContact = (id) => {
        contacts.value = contacts.value.filter(c => c.id !== id);
        saveContacts();
    };

    const setPriority = (id, priority) => {
        // If setting as primary, demote current primary
        if (priority === 'primary') {
            contacts.value.forEach(c => {
                if (c.priority === 'primary') {
                    c.priority = 'secondary';
                }
            });
        }
        updateContact(id, { priority });
    };

    const updateSettings = (newSettings) => {
        settings.value = { ...settings.value, ...newSettings };
        saveSettings();
    };

    // Emergency actions
    const makeEmergencyCall = (contact) => {
        if (!contact) return false;

        try {
            console.log('Initiating call to:', contact.name, contact.phone);
            window.location.href = `tel:${contact.phone}`;
            return true;
        } catch (error) {
            console.error('Error making call:', error);
            return false;
        }
    };

    const sendEmergencySMS = (contact, message) => {
        if (!contact) return false;

        try {
            const defaultMessage = settings.value.customMessage ||
                `🚨 EMERGENCY ALERT: Fall detected! Please check the safety monitor dashboard immediately. Time: ${new Date().toLocaleString()}`;

            const smsMessage = message || defaultMessage;
            const encodedMessage = encodeURIComponent(smsMessage);

            console.log('Sending SMS to:', contact.name, contact.phone);
            window.location.href = `sms:${contact.phone}?body=${encodedMessage}`;
            return true;
        } catch (error) {
            console.error('Error sending SMS:', error);
            return false;
        }
    };

    const triggerEmergencyProtocol = async () => {
        console.log('🚨 Emergency protocol triggered');

        if (!hasContacts.value) {
            console.warn('No emergency contacts configured');
            return {
                success: false,
                message: 'No emergency contacts configured'
            };
        }

        const actions = [];

        // Auto-call primary contact
        if (settings.value.autoCall && primaryContact.value) {
            if (settings.value.requireConfirmation) {
                // Show confirmation dialog
                const confirmed = confirm(
                    `Call ${primaryContact.value.name} (${primaryContact.value.phone})?`
                );
                if (confirmed) {
                    makeEmergencyCall(primaryContact.value);
                    actions.push(`Called ${primaryContact.value.name}`);
                }
            } else {
                makeEmergencyCall(primaryContact.value);
                actions.push(`Called ${primaryContact.value.name}`);
            }
        }

        // Auto-SMS all contacts (with delay to allow call to process)
        if (settings.value.autoSMS) {
            setTimeout(() => {
                contacts.value.forEach((contact, index) => {
                    setTimeout(() => {
                        sendEmergencySMS(contact);
                        actions.push(`SMS sent to ${contact.name}`);
                    }, index * 1000); // Stagger SMS by 1 second each
                });
            }, 2000); // Wait 2 seconds after call
        }

        return {
            success: true,
            actions: actions,
            message: `Emergency protocol activated: ${actions.join(', ')}`
        };
    };

    return {
        // State
        contacts,
        settings,
        primaryContact,
        hasContacts,

        // Methods
        addContact,
        updateContact,
        deleteContact,
        setPriority,
        updateSettings,
        validatePhone,

        // Emergency actions
        makeEmergencyCall,
        sendEmergencySMS,
        triggerEmergencyProtocol
    };
}
