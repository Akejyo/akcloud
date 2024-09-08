import { Box } from '@mui/material'
import { useState } from 'react'
import DrawerItem from './DrawerItem'

const Drawer = ({ selectedIndex, setSelectedIndex }) => {
  return (
    <>
      <Box sx={{ width: 200, height: '100vh', flexShrink: 0, backgroundColor: '#1f294a' }}>
        <DrawerItem
          index={0}
          name={'文件'}
          onClick={() => setSelectedIndex(0)}
          isSelected={0 == selectedIndex}
        />
        <DrawerItem
          index={1}
          name={'定时备份'}
          onClick={() => setSelectedIndex(1)}
          isSelected={1 == selectedIndex}
        />
        <DrawerItem
          index={2}
          name={'云端'}
          onClick={() => setSelectedIndex(2)}
          isSelected={2 == selectedIndex}
        />
      </Box>
    </>
  )
}

export default Drawer
