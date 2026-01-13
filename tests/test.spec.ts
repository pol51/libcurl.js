import { readFileSync } from 'node:fs'
import { expect, test } from '@playwright/test'

test('curl', async ({ page }) => {
  const messages: string[] = []
  const success = readFileSync('./tests/success.json', { encoding: 'utf-8' }).trim()
  const timeout = readFileSync('./tests/timeout.json', { encoding: 'utf-8' }).trim()

  const { promise, resolve } = Promise.withResolvers<void>()
  page.on('console', (msg) => {
    const text = msg.text()
    messages.push(text)
    if (text.startsWith('Completed requests:')) {
      resolve()
    }
  })

  await page.goto('http://localhost:3000/tests/test.html')
  await promise

  expect(messages[0], 'curl_escape works').toBe('https%3A%2F%2Fexample.com%2F%3Fa%3D1%26b%3D2')
  expect(messages.indexOf('HTTP code: 200'), 'Got 200').toBeGreaterThan(0)
  expect(messages.indexOf('HTTP code: 404'), 'Got 404').toBeGreaterThan(0)
  expect(messages.indexOf('HTTP code: 0'), 'Got timeout').toBeGreaterThan(0)
  expect(messages.indexOf(success), 'Got success.json').toBeGreaterThan(0)
  expect(messages.indexOf(timeout), 'Lost timeout.json').toBe(-1)
  expect(messages[messages.length - 2], 'All handles cleaned up').toBe('Remaining handles: 0')
  expect(messages[messages.length - 1], 'Completed all requests').toBe('Completed requests: 3')
})
