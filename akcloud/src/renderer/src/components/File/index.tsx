import { Box, Checkbox, Menu, MenuItem, Typography } from '@mui/material'
import { useState } from 'react'
import MoreHoriz from '@mui/icons-material/MoreHoriz'

import floderImg from '../assets/floder.png'
import fileImg from '../assets/file.png'
import { FileProps } from '@renderer/type'

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
}

const File: React.FC<ExtendedFileProps> = ({
  name,
  isDirectory,
  size,
  lastModified,
  showCheckbox,
  onCheckboxChange
}) => {
  const [anchorEl, setAnchorEl] = useState<null | HTMLElement>(null)
  const handleClick = (event: React.MouseEvent<HTMLDivElement>) => {
    setAnchorEl(event.currentTarget)
  }
  const handleClose = () => {
    setAnchorEl(null)
  }
  const handleCheckboxChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    onCheckboxChange(name, event.target.checked)
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
        <MenuItem onClick={handleClose}>数据备份</MenuItem>
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
    </Box>
  )
}
export default File
