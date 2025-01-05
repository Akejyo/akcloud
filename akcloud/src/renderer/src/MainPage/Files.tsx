import { Box, Button, Typography } from '@mui/material'
import File from '../components/File'
import AddFile from '@renderer/components/AddFile'
import { useEffect, useState } from 'react'
import { FileProps } from '@renderer/type'
import { Add } from '@mui/icons-material'
import Address from '@renderer/components/Address'

const Files = () => {
  const [files, setFiles] = useState<FileProps[]>([])
  const [showCheckbox, setShowCheckbox] = useState(false)
  const [selectedFiles, setSelectedFiles] = useState<string[]>([])
  const [addressDialogOpen, setAddressDialogOpen] = useState(false)

  useEffect(() => {
    const fetchData = async () => {
      console.log('fetchData called')
      try {
        console.log('Sending request to http://localhost:3001/api/files')
        const response = await fetch('/api/files')
        if (!response.ok) {
          throw new Error('Network response was not ok')
        }
        const data = await response.json()
        console.log('Request successful', data)
        setFiles(data)
        console.log('Files set', data)
      } catch (e) {
        console.log('Error fetching files:', e)
      }
    }

    fetchData()
  }, [])

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

  const handlePackClick = () => {
    setAddressDialogOpen(true)
  }

  const handleAddressSubmit = async (address: string) => {
    try {
      const response = await fetch('/api/files/pack', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({ files: selectedFiles, address: address })
      })
      if (!response.ok) {
        throw new Error('Network response was not ok')
      }
      const result = await response.json()
      console.log('Pack result:', result)
    } catch (error) {
      console.error('Error packing files:', error)
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
        <Button
          variant="contained"
          size="small"
          sx={{
            padding: '0px 5px !important',
            height: '40px !important',
            mr: 2
          }}
        >
          解包
        </Button>
        <AddFile />
      </Box>
      <Box sx={{ display: 'flex', flexWrap: 'wrap', gap: 2 }}>
        {files.map((file, index) => (
          <File
            key={index}
            {...file}
            showCheckbox={showCheckbox}
            onCheckboxChange={handleCheckboxChange}
          />
        ))}
      </Box>
    </Box>
  )
}
export default Files
