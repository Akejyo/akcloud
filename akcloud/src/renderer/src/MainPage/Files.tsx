import { Box, Button, Typography } from '@mui/material'
import File from '../components/File'
import AddFile from '@renderer/components/AddFile'
import { useEffect, useState } from 'react'
import { FileProps } from '@renderer/type'
import { Add } from '@mui/icons-material'
import Address from '@renderer/components/Address'
import Unpack from '@renderer/components/Unpack'
const MAX_RETRIES = 10
const RETRY_DELAY = 300
const TIMEOUT = 300
const Files = () => {
  const [files, setFiles] = useState<FileProps[]>([])
  const [showCheckbox, setShowCheckbox] = useState(false)
  const [selectedFiles, setSelectedFiles] = useState<string[]>([])
  const [addressDialogOpen, setAddressDialogOpen] = useState(false)
  const [refresh, setRefresh] = useState(false)

  useEffect(() => {
    const fetchData = async () => {
      console.log('fetchData called')
      let attempt = 0
      let success = false
      while (attempt < MAX_RETRIES && !success) {
        const controller = new AbortController()
        const timeoutId = setTimeout(() => controller.abort(), TIMEOUT)

        try {
          console.log('Sending request to http://localhost:3001/api/files')
          const response = await fetch('/api/files', { signal: controller.signal })
          clearTimeout(timeoutId)
          if (!response.ok) {
            throw new Error('Network response was not ok')
          } else {
            const data = await response.json()
            console.log('Request successful', data)
            setFiles(data)
            console.log('Files set', data)
            success = true
          }
        } catch (e) {
          attempt++
          clearTimeout(timeoutId)
          console.log('Error fetching files:', e)
          if (attempt < MAX_RETRIES) {
            await new Promise((resolve) => setTimeout(resolve, RETRY_DELAY)) // 等待一段时间后重试
          } else {
            console.log('Max retries reached. File upload failed.')
          }
        }
      }
    }

    fetchData()
  }, [refresh])

  const handleToggleCheckbox = () => {
    setShowCheckbox(!showCheckbox)
    setSelectedFiles([])
  }
  const handleCheckboxChange = (fileName: string, isChecked: boolean) => {
    setSelectedFiles((prevSelectedFiles) =>
      isChecked
        ? [...prevSelectedFiles, fileName]
        : prevSelectedFiles.filter((name) => name !== fileName)
    )
  }
  const handleRefresh = () => {
    setRefresh(!refresh)
  }

  const handlePackClick = () => {
    setAddressDialogOpen(true)
  }

  const handleAddressSubmit = async (address: string) => {
    let attempt = 0
    let success = false

    while (attempt < MAX_RETRIES && !success) {
      const controller = new AbortController()
      const timeoutId = setTimeout(() => controller.abort(), TIMEOUT)

      try {
        const response = await fetch('/api/files/pack', {
          method: 'POST',
          signal: controller.signal,
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({ files: selectedFiles, address: address })
        })
        clearTimeout(timeoutId)
        if (!response.ok) {
          throw new Error('Network response was not ok')
        } else {
          const result = await response.json()
          console.log('Pack result:', result)
          success = true
          setRefresh((prev) => !prev)
        }
      } catch (error) {
        attempt++
        clearTimeout(timeoutId)
        console.error('Error packing files:', error)
        if (attempt < MAX_RETRIES) {
          await new Promise((resolve) => setTimeout(resolve, RETRY_DELAY)) // 等待一段时间后重试
        } else {
          console.log('Max retries reached. File upload failed.')
        }
      }
    }
    setAddressDialogOpen(false)
  }

  return (
    <Box sx={{ m: 2.5, width: '100%' }}>
      <Address
        open={addressDialogOpen}
        setOpen={setAddressDialogOpen}
        onSubmit={handleAddressSubmit}
      />
      <Box sx={{ display: 'flex' }}>
        <Typography variant="h5" fontWeight="bold" sx={{ pb: 3 }}>
          文件
        </Typography>
        <Box sx={{ flexGrow: 1 }} />
        <Button
          variant="contained"
          size="small"
          sx={{
            padding: '0px 5px !important',
            height: '40px !important',
            mr: 2
          }}
          disabled={selectedFiles.length === 0}
          onClick={handlePackClick}
        >
          打包
        </Button>
        <Button
          variant="contained"
          startIcon={<Add />}
          size="small"
          sx={{
            padding: '0px 5px !important',
            height: '40px !important',
            mr: 2
          }}
          onClick={handleToggleCheckbox}
        >
          多选
        </Button>
        <Unpack setRefresh={setRefresh} />
        <AddFile setRefresh={setRefresh} />
      </Box>
      <Box sx={{ display: 'flex', flexWrap: 'wrap', gap: 2 }}>
        {files.map((file, index) => (
          <File
            key={index}
            {...file}
            showCheckbox={showCheckbox}
            onCheckboxChange={handleCheckboxChange}
            setRefresh={setRefresh}
          />
        ))}
      </Box>
    </Box>
  )
}
export default Files
