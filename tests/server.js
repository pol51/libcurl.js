import express from 'express'

const app = express()

app.use(async (req, _, next) => {
  if (req.path === '/tests/timeout.json') {
    await new Promise(r => setTimeout(r, 5000))
  }
  next()
})

app.use(express.static('.'))

app.listen(3000, () => {
  console.log('Listening on http://localhost:3000') // eslint-disable-line no-console
})
