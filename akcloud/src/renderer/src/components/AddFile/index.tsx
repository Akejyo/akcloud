import { Button, Menu, MenuItem } from '@mui/material'
import Add from '@mui/icons-material/Add'
import { useState } from 'react'
import React, { useRef } from 'react'
const MAX_RETRIES = 10
const RETRY_DELAY = 300
const TIMEOUT = 300
const Unpack = ({ setRefresh }) => {
  const [anchorEl, setAnchorEl] = useState<null | HTMLElement>(null)
  const fileInputRef = useRef<HTMLInputElement>(null)
  const floderInputRef = useRef<HTMLInputElement>(null)

  const handleClick = (event: React.MouseEvent<HTMLButtonElement>) => {
    setAnchorEl(event.currentTarget)
  }
  const handleClose = () => {
    setAnchorEl(null)
  }
  const handleFileUploadClick = () => {
    if (fileInputRef.current) {
      fileInputRef.current.click()
    }
  }
  const handleFloderUploadClick = () => {
    if (floderInputRef.current) {
      floderInputRef.current.click()
    }
  }
  const handleFileChange = async (e: React.ChangeEvent<HTMLInputElement>) => {
    const files = e.target.files
    console.log('file:', files)
    if (files && files.length > 0) {
      for (let i = 0; i < files.length; i++) {
        const formData = new FormData()
        formData.append('file', files[i])
        formData.append('relativePath', files[i].webkitRelativePath)

        let attempt = 0
        let success = false

        while (attempt < MAX_RETRIES && !success) {
          const controller = new AbortController()
          const timeoutId = setTimeout(() => controller.abort(), TIMEOUT)

          try {
            const response = await fetch('/api/files/backup', {
              method: 'POST',
              body: formData,
              signal: controller.signal
            })
            clearTimeout(timeoutId)
            if (!response.ok) {
              throw new Error('File upload failed')
            } else {
              console.log('File uploaded successfully')
              success = true
              setRefresh((prev) => !prev)
            }
          } catch (e) {
            attempt++
            clearTimeout(timeoutId)
            console.log(`Error uploading file (attempt ${attempt}):`, e)
            if (attempt < MAX_RETRIES) {
              await new Promise((resolve) => setTimeout(resolve, RETRY_DELAY)) // 等待一段时间后重试
            } else {
              console.log('Max retries reached. File upload failed.')
            }
          }
        }
      }
    }
    if (e.target) {
      e.target.value = ''
    }
    handleClose()
  }

  return (
    <>
      <Button
        variant="contained"
        startIcon={<Add />}
        size="small"
        sx={{
          padding: '0px 5px !important',
          height: '40px !important'
        }}
        onClick={handleClick}
      >
        添加文件
      </Button>
      <Menu anchorEl={anchorEl} open={Boolean(anchorEl)} onClose={handleClose}>
        <MenuItem onClick={handleFileUploadClick}>上传文件</MenuItem>
        <MenuItem onClick={handleFloderUploadClick}>上传文件夹</MenuItem>
        <input
          type="file"
          ref={fileInputRef}
          style={{ display: 'none' }}
          onChange={handleFileChange}
        />
        <input
          type="file"
          ref={floderInputRef}
          style={{ display: 'none' }}
          onChange={handleFileChange}
          {...({ directory: '', webkitdirectory: '' } as any)}
        />
      </Menu>
    </>
  )
}
export default Unpack
