import React, { useState } from 'react'
import { Dialog, DialogActions, DialogContent, DialogTitle, TextField, Button } from '@mui/material'

interface KeyProps {
  open: boolean
  setOpen: (open: boolean) => void
  onSubmit: (key: string) => void
}

const AddKey: React.FC<KeyProps> = ({ open, setOpen, onSubmit }) => {
  const [key, setKey] = useState('a'.repeat(32))

  const handleClose = () => {
    setKey('a'.repeat(32))
    setOpen(false)
  }

  const handleSubmit = () => {
    if (key.length === 32) {
      onSubmit(key)
      setKey('')
      handleClose()
    } else {
      alert('Key 必须是 32 个字符')
    }
  }

  return (
    <div>
      <Dialog open={open} onClose={handleClose}>
        <DialogTitle>输入密码(32位)</DialogTitle>
        <DialogContent>
          <TextField
            autoFocus
            margin="dense"
            label="Key"
            type="text"
            fullWidth
            variant="standard"
            value={key}
            onChange={(e) => setKey(e.target.value)}
          />
        </DialogContent>
        <DialogActions>
          <Button onClick={handleClose}>取消</Button>
          <Button onClick={handleSubmit}>提交</Button>
        </DialogActions>
      </Dialog>
    </div>
  )
}

export default AddKey
