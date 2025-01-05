import { Button, Menu, MenuItem } from '@mui/material'
import Add from '@mui/icons-material/Add'
import { useState } from 'react'
import React, { useRef } from 'react'

const AddFile = () => {
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
        try {
          const response = await fetch('/api/files/backup', {
            method: 'POST',
            body: formData
          })
          if (!response.ok) {
            throw new Error('File upload failed')
          } else {
            console.log('File uploaded successfully')
          }
        } catch (e) {
          console.log('Error uploading file:', e)
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
export default AddFile
