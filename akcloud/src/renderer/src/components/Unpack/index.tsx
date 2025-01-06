import { Button } from '@mui/material'
import React, { useEffect, useRef, useState } from 'react'
import Address from '../Address'

const Unpack = () => {
  const [addressDialogOpen, setAddressDialogOpen] = useState(false)
  const fileInputRef = useRef<HTMLInputElement>(null)
  const [file, setFile] = useState<File>()

  const handleClick = () => {
    if (fileInputRef.current) {
      fileInputRef.current.click()
    }
  }
  const handleFileChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    if (e.target.files && e.target.files.length > 0) {
      setFile(e.target.files[0])
    }
    console.log('file:', file)
    setAddressDialogOpen(true)
  }

  const handleAddressSubmit = async (address: string) => {
    if (file) {
      const formData = new FormData()
      formData.append('file', file)
      formData.append('address', address)
      console.log('address:', formData.get('address'))
      try {
        const response = await fetch('/api/files/unpack', {
          method: 'POST',
          body: formData
        })
        if (!response.ok) {
          throw new Error('File unpack failed')
        } else {
          console.log('File unpack successfully')
        }
      } catch (e) {
        console.log('Error unpacking file:', e)
      }

      setAddressDialogOpen(false)
    }
  }
  return (
    <>
      <Address
        open={addressDialogOpen}
        setOpen={setAddressDialogOpen}
        onSubmit={handleAddressSubmit}
      />
      <Button
        variant="contained"
        size="small"
        sx={{
          padding: '0px 5px !important',
          height: '40px !important',
          mr: 2
        }}
        onClick={handleClick}
      >
        解包
      </Button>
      <input
        type="file"
        ref={fileInputRef}
        style={{ display: 'none' }}
        onChange={handleFileChange}
      />
    </>
  )
}

export default Unpack
