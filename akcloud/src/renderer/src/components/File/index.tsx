import { Box, Checkbox, Menu, MenuItem, Typography } from '@mui/material'
import { useState } from 'react'
import MoreHoriz from '@mui/icons-material/MoreHoriz'

import floderImg from '../assets/floder.png'
import fileImg from '../assets/file.png'
import { FileProps } from '@renderer/type'
import AddKey from '../AddKey'
const MAX_RETRIES = 10
const RETRY_DELAY = 300
const TIMEOUT = 300
const formatDate = (timestamp: string) => {
  //只显示年月日
  const date = new Date(parseInt(timestamp, 10) * 1000)
  const year = date.getFullYear()
  const month = (date.getMonth() + 1).toString().padStart(2, '0')
  const day = date.getDate().toString().padStart(2, '0')
  return `${year}-${month}-${day}`
}

type ExtendedFileProps = FileProps & {
  showCheckbox: boolean
  onCheckboxChange: (fileName: string, isChecked: boolean) => void
  setRefresh: React.Dispatch<React.SetStateAction<boolean>>
}

const File: React.FC<ExtendedFileProps> = ({
  name,
  isDirectory,
  size,
  lastModified,
  showCheckbox,
  onCheckboxChange,
  setRefresh
}) => {
  const [anchorEl, setAnchorEl] = useState<null | HTMLElement>(null)
  const shouldShowMenuItem = name.endsWith('.hf') || name.endsWith('.lz')
  const shouldShowDecryptMenuItem = name.endsWith('.aes')
  const [encrptDialogOpen, setEncrptDialogOpen] = useState(false)
  const [decryptDialogOpen, setDecryptDialogOpen] = useState(false)

  const handleClick = (event: React.MouseEvent<HTMLDivElement>) => {
    setAnchorEl(event.currentTarget)
  }
  const handleClose = () => {
    setAnchorEl(null)
  }
  const handleCheckboxChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    onCheckboxChange(name, event.target.checked)
  }
  const handleCompress = async (method: number) => {
    console.log('压缩')
    try {
      const response = await fetch('/api/files/compress', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({ files: name, method: method.toString() })
      })
      if (!response.ok) {
        throw new Error('Network response was not ok')
      }
      const result = await response.json()
      console.log('Compress result:', result)
    } catch (error) {
      console.error('Error compressing files:', error)
    }
    handleClose()
  }
  const handleDepress = async () => {
    console.log('解压')
    let attempt = 0
    let success = false
    while (attempt < MAX_RETRIES && !success) {
      const controller = new AbortController()
      const timeoutId = setTimeout(() => controller.abort(), TIMEOUT)
      setRefresh((prev) => !prev)
      try {
        const response = await fetch('/api/files/decompress', {
          method: 'POST',
          signal: controller.signal,
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({ files: name })
        })
        clearTimeout(timeoutId)
        if (!response.ok) {
          throw new Error('Network response was not ok')
        } else {
          const result = await response.json()
          success = true
          setRefresh((prev: boolean) => !prev)
          console.log('Depress result:', result)
        }
      } catch (error) {
        attempt++
        clearTimeout(timeoutId)
        console.error('Error depressing files:', error)
        if (attempt < MAX_RETRIES) {
          await new Promise((resolve) => setTimeout(resolve, RETRY_DELAY)) // 等待一段时间后重试
        } else {
          console.log('Max retries reached. File upload failed.')
        }
      }
    }
  }
  const handleEncrypt = () => {
    setEncrptDialogOpen(true)
  }
  const handleDecrypt = () => {
    setDecryptDialogOpen(true)
  }
  const handleKeySubmit = async (key: string) => {
    setEncrptDialogOpen(false)
    let attempt = 0
    let success = false

    while (attempt < MAX_RETRIES && !success) {
      const controller = new AbortController()
      const timeoutId = setTimeout(() => controller.abort(), TIMEOUT)

      try {
        const response = await fetch('/api/files/encrypt', {
          method: 'POST',
          signal: controller.signal,
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({ files: name, key: key })
        })
        clearTimeout(timeoutId)
        if (!response.ok) {
          throw new Error('Network response was not ok')
        } else {
          const result = await response.json()
          console.log('Encrypt result:', result)
          success = true
          setRefresh((prev) => !prev)
        }
      } catch (error) {
        attempt++
        clearTimeout(timeoutId)
        console.error('Error encrypting files:', error)
        if (attempt < MAX_RETRIES) {
          await new Promise((resolve) => setTimeout(resolve, RETRY_DELAY)) // 等待一段时间后重试
        } else {
          console.log('Max retries reached. File upload failed.')
        }
      }
    }
  }
  const handleKeySubmitDecrypt = async (key: string) => {
    setDecryptDialogOpen(false)
    let attempt = 0
    let success = false

    while (attempt < MAX_RETRIES && !success) {
      const controller = new AbortController()
      const timeoutId = setTimeout(() => controller.abort(), TIMEOUT)

      try {
        const response = await fetch('/api/files/decrypt', {
          method: 'POST',
          signal: controller.signal,
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({ files: name, key: key })
        })
        clearTimeout(timeoutId)
        if (!response.ok) {
          throw new Error('Network response was not ok')
        } else {
          const result = await response.json()
          console.log('Decrypt result:', result)
          success = true
          setRefresh((prev) => !prev)
        }
      } catch (error) {
        console.error('Error decrypting files:', error)
        attempt++
        clearTimeout(timeoutId)
        if (attempt < MAX_RETRIES) {
          await new Promise((resolve) => setTimeout(resolve, RETRY_DELAY)) // 等待一段时间后重试
        } else {
          console.log('Max retries reached. File upload failed.')
        }
      }
    }
  }

  return (
    <Box
      sx={{
        px: 2,
        pt: 2,
        pb: 1,
        borderRadius: 4,
        transition: '0.2s',
        '&:hover': { backgroundColor: '#1f294a' },
        '&:hover>:first-child': { visibility: 'visible' },
        cursor: 'pointer',
        position: 'relative'
      }}
    >
      {showCheckbox && (
        <Checkbox
          sx={{ position: 'absolute', top: 0, left: 0, zIndex: 1 }}
          onChange={handleCheckboxChange}
        />
      )}

      <Box
        sx={{
          width: '30px',
          height: '30px',
          borderRadius: '50%',
          backgroundColor: 'black',
          display: 'flex',
          visibility: 'hidden',
          justifyContent: 'center',
          alignItems: 'center',
          position: 'absolute',
          right: 0,
          mr: 1,
          mt: -1
        }}
        onClick={handleClick}
      >
        <MoreHoriz />
      </Box>
      <Menu anchorEl={anchorEl} open={Boolean(anchorEl)} onClose={handleClose}>
        <MenuItem onClick={() => handleCompress(1)}>哈夫曼压缩</MenuItem>
        <MenuItem onClick={() => handleCompress(2)}>lz77压缩</MenuItem>
        {shouldShowMenuItem && <MenuItem onClick={handleDepress}>解压</MenuItem>}
        <MenuItem onClick={handleEncrypt}>加密</MenuItem>
        {shouldShowDecryptMenuItem && <MenuItem onClick={handleDecrypt}>解密</MenuItem>}
      </Menu>
      <Box>
        {isDirectory ? (
          <img src={floderImg} alt="floder" height="100px" />
        ) : (
          <img src={fileImg} alt="file" height="100px" />
        )}
        <Box
          sx={{
            display: 'flex',
            flexDirection: 'column',
            justifyContent: 'flex-end',
            alignItems: 'center',
            gap: 1
          }}
        >
          <Typography sx={{ fontSize: 14, mb: -1 }}>{name}</Typography>
          <Typography sx={{ fontSize: 12, color: 'grey' }}>{formatDate(lastModified)}</Typography>
        </Box>
      </Box>
      <AddKey open={encrptDialogOpen} setOpen={setEncrptDialogOpen} onSubmit={handleKeySubmit} />
      <AddKey
        open={decryptDialogOpen}
        setOpen={setDecryptDialogOpen}
        onSubmit={handleKeySubmitDecrypt}
      />
    </Box>
  )
}
export default File
