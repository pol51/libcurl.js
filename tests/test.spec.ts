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
  expect(messages.indexOf('HTTP code: 206'), 'Got 206 partial content').toBeGreaterThan(0)
  expect(messages.indexOf('"success" }'), 'Got partial body from range request').toBeGreaterThan(0)
  expect(messages.indexOf(success), 'Got success.json').toBeGreaterThan(0)
  expect(messages.indexOf(timeout), 'Lost timeout.json').toBe(-1)
  // CURLOPT_HEADERFUNCTION: synthesized status line is delivered first (200).
  expect(messages.some(m => m.startsWith('HDR:HTTP/1.1 200')), 'header status line (200)').toBe(true)
  // Real response headers are delivered one line per callback invocation.
  expect(messages.some(m => m.toLowerCase().startsWith('hdr:content-type')), 'content-type header line').toBe(true)
  // The trailing blank line terminates the header block.
  expect(messages.includes('HDR:'), 'trailing blank header line').toBe(true)
  // Fallback: HEADERDATA-only routes header lines to the write callback, and
  // headers are delivered even on an error status (404).
  expect(messages.some(m => m.startsWith('HDR:HTTP/1.1 404')), 'fallback header status line (404)').toBe(true)
  // A transfer with no response (timeout, status 0) delivers no header lines.
  expect(messages.some(m => m.startsWith('HDR:HTTP/1.1 0')), 'no headers on timeout').toBe(false)

  expect(messages[messages.length - 2], 'All handles cleaned up').toBe('Remaining handles: 0')
  expect(messages[messages.length - 1], 'Completed all requests').toBe('Completed requests: 4')
})
