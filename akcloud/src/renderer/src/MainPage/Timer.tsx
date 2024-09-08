import { Box, Typography } from '@mui/material'

const Timer = () => {
  return (
    <Box sx={{ m: 2.5, width: '100%' }}>
      <Box sx={{ display: 'flex' }}>
        <Typography variant="h5" fontWeight="bold" sx={{ pb: 3 }}>
          定时备份
        </Typography>
      </Box>
    </Box>
  )
}
export default Timer
