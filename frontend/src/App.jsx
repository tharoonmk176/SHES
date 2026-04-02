import { useEffect, useMemo, useState } from 'react'
import './App.css'

const DEFAULT_API_BASE = '/api'
const POLL_INTERVAL_MS = 2000

function normalizeBase(value) {
  const trimmed = value.trim()
  if (!trimmed) return DEFAULT_API_BASE
  return trimmed.replace(/\/$/, '')
}

function App() {
  const [apiBaseInput, setApiBaseInput] = useState(() => DEFAULT_API_BASE)
  const apiBase = useMemo(() => normalizeBase(apiBaseInput), [apiBaseInput])
  const [data, setData] = useState(null)
  const [lastUpdated, setLastUpdated] = useState(null)
  const [error, setError] = useState('')
  const [isLive, setIsLive] = useState(false)
  const [isSaving, setIsSaving] = useState(false)

  async function fetchData() {
    try {
      const response = await fetch(`${apiBase}/data/`, { cache: 'no-store' })
      if (!response.ok) {
        throw new Error(`HTTP ${response.status}`)
      }
      const payload = await response.json()
      setData(payload)
      setLastUpdated(new Date())
      setIsLive(true)
      setError('')
    } catch (err) {
      setIsLive(false)
      setError(`Cannot reach backend at ${apiBase} — is Django running?`)
    }
  }

  async function controlRelay(state) {
    try {
      const response = await fetch(`${apiBase}/relay/?state=${state}`, {
        method: 'POST',
      })
      if (!response.ok) {
        throw new Error(`HTTP ${response.status}`)
      }
      await fetchData()
    } catch (err) {
      setError('Control request failed — check connection.')
    }
  }

  useEffect(() => {
    fetchData()
    const id = setInterval(fetchData, POLL_INTERVAL_MS)
    return () => clearInterval(id)
  }, [apiBase])

  function handleConnect() {
    setIsSaving(true)
    setTimeout(() => {
      setIsSaving(false)
      fetchData()
    }, 150)
  }

  return (
    <div className="app">
      <header className="header">
        <div>
          <h1>Smart Hostel Energy Saver</h1>
          <p>DTL</p>
        </div>
        <div className="badge-live">
          <span className={`dot ${isLive ? '' : 'red'}`} />
          <span>{isLive ? 'Live' : 'Disconnected'}</span>
        </div>
      </header>

      <div className="api-bar">
        <label htmlFor="apiBase">Backend API:</label>
        <input
          id="apiBase"
          type="text"
          value={apiBaseInput}
          onChange={(event) => setApiBaseInput(event.target.value)}
        />
        <button className="btn-save" onClick={handleConnect}>
          {isSaving ? 'Saving…' : 'Connect'}
        </button>
        <span className="conn-msg">{isSaving ? 'Saved — reconnecting…' : ''}</span>
      </div>

      <div className="grid">
        <div className="card">
          <div className="card-label">Relay / Light</div>
          <div className={`card-val ${data?.relay ? 'val-green' : 'val-red'}`}>
            {data ? (data.relay ? 'ON' : 'OFF') : '--'}
          </div>
          <span className={`pill ${data ? (data.relay ? 'green' : 'red') : 'gray'}`}>
            {data ? (data.relay ? 'Active' : 'Inactive') : '--'}
          </span>
        </div>

        <div className="card">
          <div className="card-label">PIR Sensor</div>
          <div className={`card-val ${data?.motion ? 'val-green' : 'val-purple'}`}>
            {data ? (data.motion ? 'Detected' : 'Idle') : '--'}
          </div>
          <span className={`pill ${data ? (data.motion ? 'green' : 'gray') : 'gray'}`}>
            {data ? (data.motion ? 'Heat detected' : 'No heat') : '--'}
          </span>
        </div>

        <div className="card">
          <div className="card-label">Motion Count</div>
          <div className="card-val val-amber">{data ? data.motionCount : '--'}</div>
          <span className="pill gray">total today</span>
        </div>

        <div className="card">
          <div className="card-label">LDR Value</div>
          <div className="card-val val-teal">{data ? data.ldr : '--'}</div>
          <span className={`pill ${data ? (data.isDark ? 'green' : 'red') : 'gray'}`}>
            {data ? (data.isDark ? 'Dark' : 'Bright') : '--'}
          </span>
        </div>
      </div>

      <div className="event-row">
        <div>
          <div className="lbl">Last Event</div>
          <div className="txt">{data ? data.lastEvent : 'Waiting for data...'}</div>
        </div>
        <div className="time">
          {lastUpdated ? `Updated: ${lastUpdated.toLocaleTimeString()}` : '--'}
        </div>
      </div>

      {data?.override && (
        <div id="overrideBar">Manual override is active — auto logic paused</div>
      )}

      <div className="controls">
        <button className="btn btn-on" onClick={() => controlRelay('on')}>
          Force ON
        </button>
        <button className="btn btn-off" onClick={() => controlRelay('off')}>
          Force OFF
        </button>
      </div>

      <div className="err-msg">{error}</div>
    </div>
  )
}

export default App
