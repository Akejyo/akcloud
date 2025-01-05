import React, { useState } from 'react'
import { Dialog, DialogActions, DialogContent, DialogTitle, TextField, Button } from '@mui/material'

interface AddressProps {
  open: boolean
  setOpen: (open: boolean) => void
  onSubmit: (address: string) => void
}

const Address: React.FC<AddressProps> = ({ open, setOpen, onSubmit }) => {
  const [address, setAddress] = useState('')

  const handleClose = () => {
    setOpen(false)
  }

  const handleSubmit = () => {
    onSubmit(address)
    setAddress('')
    handleClose()
  }

  return (
    <div>
      <Dialog open={open} onClose={handleClose}>
        <DialogTitle>输入地址</DialogTitle>
        <DialogContent>
          <TextField
            autoFocus
            margin="dense"
            label="地址"
            type="text"
            fullWidth
            variant="standard"
            value={address}
            onChange={(e) => setAddress(e.target.value)}
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

export default Address
