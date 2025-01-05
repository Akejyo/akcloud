import { Box, Typography } from '@mui/material'
import Folder from '@mui/icons-material/FolderCopy'
import AvTimer from '@mui/icons-material/AvTimer'
import CloudUpload from '@mui/icons-material/CloudUpload'
import { ReactElement } from 'react'
interface DrawerItemProps {
  index: number
  name: string
  onClick?: () => void
  selectedIndex: number
}

const icons: ReactElement[] = [<Folder />, <AvTimer />, <CloudUpload />]

const DrawerItem: React.FC<DrawerItemProps> = ({ index, name, onClick, selectedIndex }) => {
  return (
    <Box sx={{ p: 1 }}>
      <Box
        sx={{
          borderRadius: 1,
          display: 'flex',
          p: 1,
          transition: '0.3s',
          cursor: 'pointer',
          '&:hover': {
            backgroundColor: '#455175'
          },
          backgroundColor: selectedIndex == index ? '#2a3f73' : '#1f294a',
          alignItems: 'center'
        }}
        onClick={onClick}
      >
        {icons[index]}
        <Typography sx={{ fontSize: 14, pl: 2 }}>{name}</Typography>
      </Box>
    </Box>
  )
}
export default DrawerItem
