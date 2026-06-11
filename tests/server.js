import express from 'express'

const app = express()

app.use(async (req, _, next) => {
  if (req.path === '/tests/timeout.json') {
    await new Promise(r => setTimeout(r, 5000))
  }
  next()
})

// Reflect a custom request header back as a response header so tests can
// assert that CURLOPT_HTTPHEADER actually put the header on the wire.
app.get('/tests/echo', (req, res) => {
  res.set('X-Echo-Custom', req.get('X-Custom-Header') ?? '')
  // "X-Empty-Header;" (semicolon form) must arrive as a present, empty header.
  res.set('X-Echo-Empty', 'x-empty-header' in req.headers ? 'present' : 'absent')
  // "X-Removed-Header:" (empty value) must be suppressed entirely.
  res.set('X-Echo-Removed', 'x-removed-header' in req.headers ? 'present' : 'absent')
  res.json({ received: req.get('X-Custom-Header') ?? null })
})

app.use(express.static('.'))

app.listen(3000, () => {
  console.log('Listening on http://localhost:3000') // eslint-disable-line no-console
})
