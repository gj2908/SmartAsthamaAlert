<template>
  <q-layout view="lHh Lpr lFf" class="main-layout">
    <!-- Enhanced Glassmorphism Header -->
    <q-header class="glass-header text-white">
      <q-toolbar class="q-py-sm">
        <q-btn 
          flat 
          dense 
          round 
          icon="menu" 
          aria-label="Menu" 
          @click="toggleLeftDrawer" 
          class="menu-btn"
        />

        <q-avatar size="32px" class="q-ml-md glass-icon">
          <q-icon name="health_and_safety" color="white" size="20px" />
        </q-avatar>

        <q-toolbar-title class="text-weight-light tracking-wide">
          <span class="text-weight-bold">IoT</span> Dashboard
        </q-toolbar-title>

        <div class="row items-center q-gutter-sm">
          <q-btn round flat dense icon="notifications_none" class="glass-btn-round">
            <q-badge color="red" floating rounded dot />
          </q-btn>
          <q-btn round flat dense icon="settings" class="glass-btn-round" to="/settings">
            <q-tooltip>Settings</q-tooltip>
          </q-btn>
          <div class="user-profile q-ml-sm row items-center glass-pill">
            <q-avatar size="24px" class="bg-primary text-white">A</q-avatar>
            <span class="text-caption q-ml-sm q-mr-xs text-weight-medium">Admin</span>
          </div>
        </div>
      </q-toolbar>
    </q-header>

    <!-- Enhanced Dark Sidebar -->
    <q-drawer 
      v-model="leftDrawerOpen" 
      bordered 
      behavior="desktop"
      class="dark-sidebar"
      :width="260"
    >
      <div class="column full-height">
        <div class="drawer-header q-pa-lg">
          <div class="text-overline text-grey-5">NAVIGATION</div>
        </div>

        <q-list padding class="menu-list">
          <q-item 
            clickable 
            v-ripple 
            to="/" 
            exact 
            active-class="active-menu-item"
            class="menu-item q-mb-sm"
          >
            <q-item-section avatar>
              <q-icon name="dashboard" />
            </q-item-section>
            <q-item-section>
              <q-item-label>Overview</q-item-label>
              <q-item-label caption class="text-grey-5">Live Monitoring</q-item-label>
            </q-item-section>
          </q-item>


          <q-item 
            clickable 
            v-ripple 
            to="/settings" 
            active-class="active-menu-item"
            class="menu-item q-mb-sm"
          >
            <q-item-section avatar>
              <q-icon name="contact_phone" />
            </q-item-section>
            <q-item-section>
              <q-item-label>Emergency Contacts</q-item-label>
              <q-item-label caption class="text-grey-5">Settings</q-item-label>
            </q-item-section>
          </q-item>

          <q-separator class="q-my-md bg-grey-9" />

          <q-item-label header class="text-grey-6 q-pl-md q-pb-sm text-size-xs">SYSTEM</q-item-label>

          <q-item clickable v-ripple class="menu-item disabled-item">
            <q-item-section avatar>
              <q-icon name="analytics" />
            </q-item-section>
            <q-item-section>
              <q-item-label>Analytics</q-item-label>
              <q-item-label caption>History Logs (Pro)</q-item-label>
            </q-item-section>
          </q-item>

          <q-item clickable v-ripple class="menu-item disabled-item">
            <q-item-section avatar>
              <q-icon name="manage_accounts" />
            </q-item-section>
            <q-item-section>
              <q-item-label>Users</q-item-label>
              <q-item-label caption>Access Control</q-item-label>
            </q-item-section>
          </q-item>
        </q-list>

        <div class="col-grow"></div>

        <div class="drawer-footer q-pa-md">
          <div class="system-status-card">
            <div class="row items-center justify-between q-mb-xs">
              <span class="text-caption text-grey-4">Server Status</span>
              <span class="text-caption text-green-4">Online</span>
            </div>
            <div class="status-dot"></div>
          </div>
        </div>
      </div>
    </q-drawer>

    <q-page-container>
      <router-view />
    </q-page-container>
  </q-layout>
</template>

<script setup>
import { ref } from 'vue'

const leftDrawerOpen = ref(false) // Collapsed by default

function toggleLeftDrawer() {
  leftDrawerOpen.value = !leftDrawerOpen.value
}
</script>

<style scoped>
.main-layout {
  background: #0f121a; /* Match the darkest dashboard color */
}

/* Header Styling */
.glass-header {
  background: rgba(15, 18, 26, 0.8);
  backdrop-filter: blur(12px);
  border-bottom: 1px solid rgba(255, 255, 255, 0.05);
}

.tracking-wide {
  letter-spacing: 1px;
}

.glass-icon {
  background: rgba(255, 255, 255, 0.1);
  border-radius: 8px;
}

.glass-btn-round {
  color: #a0aec0;
  transition: all 0.3s ease;
}

.glass-btn-round:hover {
  color: white;
  background: rgba(255, 255, 255, 0.1);
}

.glass-pill {
  background: rgba(255, 255, 255, 0.05);
  border-radius: 30px;
  padding: 4px 12px 4px 4px;
  border: 1px solid rgba(255, 255, 255, 0.05);
}

/* Sidebar Styling */
.dark-sidebar {
  background: #151923 !important; /* Slightly lighter than main bg */
  color: #e2e8f0;
  border-right: 1px solid rgba(255, 255, 255, 0.05);
}

.menu-list {
  padding: 16px;
}

.menu-item {
  border-radius: 12px;
  color: #a0aec0;
  transition: all 0.3s ease;
}

.menu-item:hover {
  background: rgba(255, 255, 255, 0.03);
  color: white;
}

.active-menu-item {
  background: linear-gradient(90deg, rgba(66, 153, 225, 0.15) 0%, rgba(66, 153, 225, 0.05) 100%);
  color: #63b3ed;
  border-left: 3px solid #63b3ed;
}

.active-menu-item .text-caption {
  color: #90cdf4 !important;
}

.disabled-item {
  opacity: 0.5;
  cursor: not-allowed;
}

.text-size-xs {
  font-size: 0.75rem;
  font-weight: 600;
  letter-spacing: 0.05em;
}

/* System Status in Drawer */
.system-status-card {
  background: rgba(0, 0, 0, 0.2);
  border-radius: 12px;
  padding: 12px;
  border: 1px solid rgba(255, 255, 255, 0.05);
  position: relative;
  overflow: hidden;
}

.status-dot {
  height: 2px;
  width: 100%;
  background: linear-gradient(90deg, transparent, #48bb78, transparent);
  margin-top: 4px;
  opacity: 0.7;
}
</style>
