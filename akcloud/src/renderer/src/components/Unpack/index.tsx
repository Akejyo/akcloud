import { Button } from '@mui/material'
import React, { useRef, useState } from 'react'
import Address from '../Address'
const MAX_RETRIES = 10
const RETRY_DELAY = 300
const TIMEOUT = 300
const Unpack = ({ setRefresh }) => {
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
      let attempt = 0
      let success = false

      while (attempt < MAX_RETRIES && !success) {
        const controller = new AbortController()
        const timeoutId = setTimeout(() => controller.abort(), TIMEOUT)

        try {
          const response = await fetch('/api/files/unpack', {
            method: 'POST',
            body: formData,
            signal: controller.signal
          })
          clearTimeout(timeoutId)
          if (!response.ok) {
            throw new Error('File unpack failed')
          } else {
            console.log('File unpack successfully')
            success = true
            setRefresh((prev) => !prev)
          }
        } catch (e) {
          attempt++
          clearTimeout(timeoutId)
          console.log('Error unpacking file:', e)
          if (attempt < MAX_RETRIES) {
            await new Promise((resolve) => setTimeout(resolve, RETRY_DELAY)) // 等待一段时间后重试
          } else {
            console.log('Max retries reached. File upload failed.')
          }
        }
      }
      setRefresh((prev) => !prev)
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
