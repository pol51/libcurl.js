import { defineConfig, devices } from '@playwright/test';

const mobileTests = /.*\.mobile\.spec\.ts/

export default defineConfig({
  testDir: 'tests',
  retries: 0,
  webServer: {
    command: 'pnpm run serve',
    port: 3000,
    reuseExistingServer: true
  }
})
