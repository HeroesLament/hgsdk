// hgrfdigicali_ah.h
#ifndef _HGRFDIGICALI_AH_H_
#define _HGRFDIGICALI_AH_H_

struct _DCIMB
{
    __IO uint32_t DC[6];
    __IO uint32_t IMB[6];
};

struct hgrfdigicali_reg
{
    __IO uint32_t RFDCOCCON0;
    __IO uint32_t RFDCOCCON1;
    __IO uint32_t RFDCOCCON2;
    __IO uint32_t RFDCOCCON3;
    __IO uint32_t RFDCOCCON4;
    __IO uint32_t RFPWRCON0;
    __IO uint32_t RFPWRCON1;
    __IO uint32_t RFPWRCON2;
    __IO uint32_t XOSCDFMCON0;
    __IO uint32_t XOSCDFMCON1;
    __IO uint32_t XOSCDFMCON2;
    __IO uint32_t TXPWRIDX;    // 0x002c
    struct _DCIMB TXDCIMB;     // 0x0030~0x005C
    __IO uint32_t RXPWRIDX;    // 0x0060
    struct _DCIMB RXDCIMB[4];  // 0x0064~0x0120
    struct _DCIMB RXFBDCIMB;   // 0x0124~0x0150
    __IO uint32_t RXFILTER;    // 0x0154
    __IO uint32_t TXDIGPWR01;  // 0x0158
    __IO uint32_t TXDIGPWR23;  // 0x015c
    __IO uint32_t TXDIGPWR45;  // 0x0160
};

struct hgrfdigicali {
    struct rfdigicali_device     dev;
    struct hgrfdigicali_reg      *prfdigicalireg;
};

// Determine whether rx filter is controlled by adc enable signal.
// 0: rx filter is controlled by filter_en
// 1: rx filter is controlled by filter_en and adc_en
#define K_FB_FLT_ADC_CTL   1

//------------------------------------------------------------------------------
// ah rf digital calibration register definition
//------------------------------------------------------------------------------
// Register Address: 'h0000
#define RFDCOCCON0_PHY_TRI_EN_MSK       0x02000000
#define RFDCOCCON0_PHY_TRI_EN_SHIFT     25
#define RFDCOCCON0_COARSE_TIME_MSK      0x01FE0000
#define RFDCOCCON0_COARSE_TIME_SHIFT    17
#define RFDCOCCON0_FINE_STEP_MSK        0x0001E000
#define RFDCOCCON0_FINE_STEP_SHIFT      13
#define RFDCOCCON0_COARSE_STEP_MSK      0x00001E00
#define RFDCOCCON0_COARSE_STEP_SHIFT    9
#define RFDCOCCON0_COARSE_EN_MSK        0x00000100
#define RFDCOCCON0_COARSE_EN_SHIFT      8
#define RFDCOCCON0_TRACKING_EN_MSK      0x00000080
#define RFDCOCCON0_TRACKING_EN_SHIFT    7
#define RFDCOCCON0_FRZ_CALIB_MSK        0x00000040
#define RFDCOCCON0_FRZ_CALIB_SHIFT      6
#define RFDCOCCON0_ACC_HOLD_MSK         0x00000020
#define RFDCOCCON0_ACC_HOLD_SHIFT       5
#define RFDCOCCON0_ACC_RELOAD_MSK       0x00000010
#define RFDCOCCON0_ACC_RELOAD_SHIFT     4
#define RFDCOCCON0_ACC_CLR_MSK          0x00000008
#define RFDCOCCON0_ACC_CLR_SHIFT        3
#define RFDCOCCON0_TXRX_SEL_MSK         0x00000004
#define RFDCOCCON0_TXRX_SEL_SHIFT       2
#define RFDCOCCON0_MEN_Q_MSK            0x00000002
#define RFDCOCCON0_MEN_Q_SHIFT          1
#define RFDCOCCON0_MEN_I_MSK            0x00000001
#define RFDCOCCON0_MEN_I_SHIFT          0

// Register Address: 'h0004
#define RFDCOCCON1_CALIB_TIME_MSK      0x00FFFF00
#define RFDCOCCON1_CALIB_TIME_SHIFT    8
#define RFDCOCCON1_WAIT_TIME_MSK       0x000000FF
#define RFDCOCCON1_WAIT_TIME_SHIFT     0

// Register Address: 'h0008
#define RFDCOCCON2_ACC_RLDVAL_Q_MSK      0x00FFF000
#define RFDCOCCON2_ACC_RLDVAL_Q_SHIFT    12
#define RFDCOCCON2_ACC_RLDVAL_I_MSK      0x00000FFF
#define RFDCOCCON2_ACC_RLDVAL_I_SHIFT    0

// Register Address: 'h000C
#define RFDCOCCON3_DC_EST_Q_MSK      0x00FFF000
#define RFDCOCCON3_DC_EST_Q_SHIFT    12
#define RFDCOCCON3_DC_EST_I_MSK      0x00000FFF
#define RFDCOCCON3_DC_EST_I_SHIFT    0

// Register Address: 'h0010
#define RFDCOCCON4_RXDOUT_SEL_MSK        0x0C000000
#define RFDCOCCON4_RXDOUT_SEL_SHIFT      26
#define RFDCOCCON4_DC_VALUE_SEL_MSK      0x02000000
#define RFDCOCCON4_DC_VALUE_SEL_SHIFT    25
#define RFDCOCCON4_DC_VALUE_Q_MSK        0x00FFF000
#define RFDCOCCON4_DC_VALUE_Q_SHIFT      12
#define RFDCOCCON4_DC_VALUE_I_MSK        0x00000FFF
#define RFDCOCCON4_DC_VALUE_I_SHIFT      0

// Register Address: 'h0014
#define RFPWRCON0_PWR_WAIT_TIME_MSK      0x3FFE0000
#define RFPWRCON0_PWR_WAIT_TIME_SHIFT    17
#define RFPWRCON0_PWR_CALCMODE_MSK       0x00010000
#define RFPWRCON0_PWR_CALCMODE_SHIFT     16
#define RFPWRCON0_PHY_TRI_EN_MSK         0x00008000
#define RFPWRCON0_PHY_TRI_EN_SHIFT       15
#define RFPWRCON0_MAG_PND_MSK            0x00004000
#define RFPWRCON0_MAG_PND_SHIFT          14
#define RFPWRCON0_PWR_PND_MSK            0x00002000
#define RFPWRCON0_PWR_PND_SHIFT          13
#define RFPWRCON0_MAG_DCOCSEL_MSK        0x00000800
#define RFPWRCON0_MAG_DCOCSEL_SHIFT      11
#define RFPWRCON0_MAG_AVG_SEL_MSK        0x00000780
#define RFPWRCON0_MAG_AVG_SEL_SHIFT      7
#define RFPWRCON0_MAG_MEN_MSK            0x00000040
#define RFPWRCON0_MAG_MEN_SHIFT          6
#define RFPWRCON0_PWR_DATASEL_MSK        0x00000020
#define RFPWRCON0_PWR_DATASEL_SHIFT      5
#define RFPWRCON0_PWR_AVG_SEL_MSK        0x0000001E
#define RFPWRCON0_PWR_AVG_SEL_SHIFT      1
#define RFPWRCON0_PWR_MEN_MSK            0x00000001
#define RFPWRCON0_PWR_MEN_SHIFT          0

// Register Address: 'h0018
#define RFPWRCON1_PWR_LIN_MSK      0x7FFFFF00
#define RFPWRCON1_PWR_LIN_SHIFT    8
#define RFPWRCON1_PWR_DB_MSK       0x000000FF
#define RFPWRCON1_PWR_DB_SHIFT     0

// Register Address: 'h001C
#define RFPWRCON2_MAG_LIN_MSK      0x3FFFFF00
#define RFPWRCON2_MAG_LIN_SHIFT    8
#define RFPWRCON2_MAG_DB_MSK       0x000000FF
#define RFPWRCON2_MAG_DB_SHIFT     0

// Register Address: 'h0020
#define XOSCDFMCON0_DFM_FLAG_MSK       0x00000080
#define XOSCDFMCON0_DFM_FLAG_SHIFT     7
#define XOSCDFMCON0_SYNC_TIME_MSK      0x0000007E
#define XOSCDFMCON0_SYNC_TIME_SHIFT    1
#define XOSCDFMCON0_DFM_MEN_MSK        0x00000001
#define XOSCDFMCON0_DFM_MEN_SHIFT      0

// Register Address: 'h0024
#define XOSCDFMCON1_DFM_REF_TIME_MSK      0xFFFFFFFF
#define XOSCDFMCON1_DFM_REF_TIME_SHIFT    0

// Register Address: 'h0028
#define XOSCDFMCON2_DFM_ACT_CNT_MSK      0xFFFFFFFF
#define XOSCDFMCON2_DFM_ACT_CNT_SHIFT    0

// Register Address: 'h2c
#define TXPWRIDX_PWR_IDX_SRC_MSK      0x00008000
#define TXPWRIDX_PWR_IDX_SRC_SHIFT    15
#define TXPWRIDX_WRITE_IND_MSK        0x00000008
#define TXPWRIDX_WRITE_IND_SHIFT      3
#define TXPWRIDX_TX_PWR_IDX_MSK       0x00000007
#define TXPWRIDX_TX_PWR_IDX_SHIFT     0

// Register Address: 'h30
#define TXDC0_TX_PHASE_COMP_MSK      0x1FF00000
#define TXDC0_TX_PHASE_COMP_SHIFT    20
#define TXDC0_TX_Q_DCOFS_MSK         0x000FFC00
#define TXDC0_TX_Q_DCOFS_SHIFT       10
#define TXDC0_TX_I_DCOFS_MSK         0x000003FF
#define TXDC0_TX_I_DCOFS_SHIFT       0

// Register Address: 'h34
#define TXDC1_TX_PHASE_COMP_MSK      0x1FF00000
#define TXDC1_TX_PHASE_COMP_SHIFT    20
#define TXDC1_TX_Q_DCOFS_MSK         0x000FFC00
#define TXDC1_TX_Q_DCOFS_SHIFT       10
#define TXDC1_TX_I_DCOFS_MSK         0x000003FF
#define TXDC1_TX_I_DCOFS_SHIFT       0

// Register Address: 'h38
#define TXDC2_TX_PHASE_COMP_MSK      0x1FF00000
#define TXDC2_TX_PHASE_COMP_SHIFT    20
#define TXDC2_TX_Q_DCOFS_MSK         0x000FFC00
#define TXDC2_TX_Q_DCOFS_SHIFT       10
#define TXDC2_TX_I_DCOFS_MSK         0x000003FF
#define TXDC2_TX_I_DCOFS_SHIFT       0

// Register Address: 'h3c
#define TXDC3_TX_PHASE_COMP_MSK      0x1FF00000
#define TXDC3_TX_PHASE_COMP_SHIFT    20
#define TXDC3_TX_Q_DCOFS_MSK         0x000FFC00
#define TXDC3_TX_Q_DCOFS_SHIFT       10
#define TXDC3_TX_I_DCOFS_MSK         0x000003FF
#define TXDC3_TX_I_DCOFS_SHIFT       0

// Register Address: 'h40
#define TXDC4_TX_PHASE_COMP_MSK      0x1FF00000
#define TXDC4_TX_PHASE_COMP_SHIFT    20
#define TXDC4_TX_Q_DCOFS_MSK         0x000FFC00
#define TXDC4_TX_Q_DCOFS_SHIFT       10
#define TXDC4_TX_I_DCOFS_MSK         0x000003FF
#define TXDC4_TX_I_DCOFS_SHIFT       0

// Register Address: 'h44
#define TXDC5_TX_PHASE_COMP_MSK      0x1FF00000
#define TXDC5_TX_PHASE_COMP_SHIFT    20
#define TXDC5_TX_Q_DCOFS_MSK         0x000FFC00
#define TXDC5_TX_Q_DCOFS_SHIFT       10
#define TXDC5_TX_I_DCOFS_MSK         0x000003FF
#define TXDC5_TX_I_DCOFS_SHIFT       0

// Register Address: 'h48
#define TXIMB0_TX_GAIN_COMP_REC_MSK      0x00FFF000
#define TXIMB0_TX_GAIN_COMP_REC_SHIFT    12
#define TXIMB0_TX_GAIN_COMP_MSK          0x00000FFF
#define TXIMB0_TX_GAIN_COMP_SHIFT        0

// Register Address: 'h4c
#define TXIMB1_TX_GAIN_COMP_REC_MSK      0x00FFF000
#define TXIMB1_TX_GAIN_COMP_REC_SHIFT    12
#define TXIMB1_TX_GAIN_COMP_MSK          0x00000FFF
#define TXIMB1_TX_GAIN_COMP_SHIFT        0

// Register Address: 'h50
#define TXIMB2_TX_GAIN_COMP_REC_MSK      0x00FFF000
#define TXIMB2_TX_GAIN_COMP_REC_SHIFT    12
#define TXIMB2_TX_GAIN_COMP_MSK          0x00000FFF
#define TXIMB2_TX_GAIN_COMP_SHIFT        0

// Register Address: 'h54
#define TXIMB3_TX_GAIN_COMP_REC_MSK      0x00FFF000
#define TXIMB3_TX_GAIN_COMP_REC_SHIFT    12
#define TXIMB3_TX_GAIN_COMP_MSK          0x00000FFF
#define TXIMB3_TX_GAIN_COMP_SHIFT        0

// Register Address: 'h58
#define TXIMB4_TX_GAIN_COMP_REC_MSK      0x00FFF000
#define TXIMB4_TX_GAIN_COMP_REC_SHIFT    12
#define TXIMB4_TX_GAIN_COMP_MSK          0x00000FFF
#define TXIMB4_TX_GAIN_COMP_SHIFT        0

// Register Address: 'h5c
#define TXIMB5_TX_GAIN_COMP_REC_MSK      0x00FFF000
#define TXIMB5_TX_GAIN_COMP_REC_SHIFT    12
#define TXIMB5_TX_GAIN_COMP_MSK          0x00000FFF
#define TXIMB5_TX_GAIN_COMP_SHIFT        0

// Register Address: 'h60
#define RXPWRIDX_FB_COMP_EN_MSK           0x00000400
#define RXPWRIDX_FB_COMP_EN_SHIFT         10
#define RXPWRIDX_RF_RX_BW_MSK             0x00000300
#define RXPWRIDX_RF_RX_BW_SHIFT           8
#define RXPWRIDX_RX_DCOC_PWR_IDX_MSK      0x000000E0
#define RXPWRIDX_RX_DCOC_PWR_IDX_SHIFT    5
#define RXPWRIDX_PWR_IDX_SRC_MSK          0x00000010
#define RXPWRIDX_PWR_IDX_SRC_SHIFT        4
#define RXPWRIDX_WRITE_IND_MSK            0x00000008
#define RXPWRIDX_WRITE_IND_SHIFT          3
#define RXPWRIDX_RX_PWR_IDX_MSK           0x00000007
#define RXPWRIDX_RX_PWR_IDX_SHIFT         0

// Register Address: 'h64
#define RX1MDC0_RX_PHASE_COMP_MSK      0x3FF00000
#define RX1MDC0_RX_PHASE_COMP_SHIFT    20
#define RX1MDC0_RX_Q_DCOFS_MSK         0x000FFC00
#define RX1MDC0_RX_Q_DCOFS_SHIFT       10
#define RX1MDC0_RX_I_DCOFS_MSK         0x000003FF
#define RX1MDC0_RX_I_DCOFS_SHIFT       0

// Register Address: 'h68
#define RX1MDC1_RX_PHASE_COMP_MSK      0x3FF00000
#define RX1MDC1_RX_PHASE_COMP_SHIFT    20
#define RX1MDC1_RX_Q_DCOFS_MSK         0x000FFC00
#define RX1MDC1_RX_Q_DCOFS_SHIFT       10
#define RX1MDC1_RX_I_DCOFS_MSK         0x000003FF
#define RX1MDC1_RX_I_DCOFS_SHIFT       0

// Register Address: 'h6c
#define RX1MDC2_RX_PHASE_COMP_MSK      0x3FF00000
#define RX1MDC2_RX_PHASE_COMP_SHIFT    20
#define RX1MDC2_RX_Q_DCOFS_MSK         0x000FFC00
#define RX1MDC2_RX_Q_DCOFS_SHIFT       10
#define RX1MDC2_RX_I_DCOFS_MSK         0x000003FF
#define RX1MDC2_RX_I_DCOFS_SHIFT       0

// Register Address: 'h70
#define RX1MDC3_RX_PHASE_COMP_MSK      0x3FF00000
#define RX1MDC3_RX_PHASE_COMP_SHIFT    20
#define RX1MDC3_RX_Q_DCOFS_MSK         0x000FFC00
#define RX1MDC3_RX_Q_DCOFS_SHIFT       10
#define RX1MDC3_RX_I_DCOFS_MSK         0x000003FF
#define RX1MDC3_RX_I_DCOFS_SHIFT       0

// Register Address: 'h74
#define RX1MDC4_RX_PHASE_COMP_MSK      0x3FF00000
#define RX1MDC4_RX_PHASE_COMP_SHIFT    20
#define RX1MDC4_RX_Q_DCOFS_MSK         0x000FFC00
#define RX1MDC4_RX_Q_DCOFS_SHIFT       10
#define RX1MDC4_RX_I_DCOFS_MSK         0x000003FF
#define RX1MDC4_RX_I_DCOFS_SHIFT       0

// Register Address: 'h78
#define RX1MDC5_RX_PHASE_COMP_MSK      0x3FF00000
#define RX1MDC5_RX_PHASE_COMP_SHIFT    20
#define RX1MDC5_RX_Q_DCOFS_MSK         0x000FFC00
#define RX1MDC5_RX_Q_DCOFS_SHIFT       10
#define RX1MDC5_RX_I_DCOFS_MSK         0x000003FF
#define RX1MDC5_RX_I_DCOFS_SHIFT       0

// Register Address: 'h7c
#define RX1MIMB0_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX1MIMB0_RX_GAIN_COMP_REC_SHIFT    12
#define RX1MIMB0_RX_GAIN_COMP_MSK          0x00000FFF
#define RX1MIMB0_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h80
#define RX1MIMB1_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX1MIMB1_RX_GAIN_COMP_REC_SHIFT    12
#define RX1MIMB1_RX_GAIN_COMP_MSK          0x00000FFF
#define RX1MIMB1_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h84
#define RX1MIMB2_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX1MIMB2_RX_GAIN_COMP_REC_SHIFT    12
#define RX1MIMB2_RX_GAIN_COMP_MSK          0x00000FFF
#define RX1MIMB2_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h88
#define RX1MIMB3_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX1MIMB3_RX_GAIN_COMP_REC_SHIFT    12
#define RX1MIMB3_RX_GAIN_COMP_MSK          0x00000FFF
#define RX1MIMB3_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h8c
#define RX1MIMB4_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX1MIMB4_RX_GAIN_COMP_REC_SHIFT    12
#define RX1MIMB4_RX_GAIN_COMP_MSK          0x00000FFF
#define RX1MIMB4_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h90
#define RX1MIMB5_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX1MIMB5_RX_GAIN_COMP_REC_SHIFT    12
#define RX1MIMB5_RX_GAIN_COMP_MSK          0x00000FFF
#define RX1MIMB5_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h94
#define RX2MDC0_RX_PHASE_COMP_MSK      0x3FF00000
#define RX2MDC0_RX_PHASE_COMP_SHIFT    20
#define RX2MDC0_RX_Q_DCOFS_MSK         0x000FFC00
#define RX2MDC0_RX_Q_DCOFS_SHIFT       10
#define RX2MDC0_RX_I_DCOFS_MSK         0x000003FF
#define RX2MDC0_RX_I_DCOFS_SHIFT       0

// Register Address: 'h98
#define RX2MDC1_RX_PHASE_COMP_MSK      0x3FF00000
#define RX2MDC1_RX_PHASE_COMP_SHIFT    20
#define RX2MDC1_RX_Q_DCOFS_MSK         0x000FFC00
#define RX2MDC1_RX_Q_DCOFS_SHIFT       10
#define RX2MDC1_RX_I_DCOFS_MSK         0x000003FF
#define RX2MDC1_RX_I_DCOFS_SHIFT       0

// Register Address: 'h9c
#define RX2MDC2_RX_PHASE_COMP_MSK      0x3FF00000
#define RX2MDC2_RX_PHASE_COMP_SHIFT    20
#define RX2MDC2_RX_Q_DCOFS_MSK         0x000FFC00
#define RX2MDC2_RX_Q_DCOFS_SHIFT       10
#define RX2MDC2_RX_I_DCOFS_MSK         0x000003FF
#define RX2MDC2_RX_I_DCOFS_SHIFT       0

// Register Address: 'ha0
#define RX2MDC3_RX_PHASE_COMP_MSK      0x3FF00000
#define RX2MDC3_RX_PHASE_COMP_SHIFT    20
#define RX2MDC3_RX_Q_DCOFS_MSK         0x000FFC00
#define RX2MDC3_RX_Q_DCOFS_SHIFT       10
#define RX2MDC3_RX_I_DCOFS_MSK         0x000003FF
#define RX2MDC3_RX_I_DCOFS_SHIFT       0

// Register Address: 'ha4
#define RX2MDC4_RX_PHASE_COMP_MSK      0x3FF00000
#define RX2MDC4_RX_PHASE_COMP_SHIFT    20
#define RX2MDC4_RX_Q_DCOFS_MSK         0x000FFC00
#define RX2MDC4_RX_Q_DCOFS_SHIFT       10
#define RX2MDC4_RX_I_DCOFS_MSK         0x000003FF
#define RX2MDC4_RX_I_DCOFS_SHIFT       0

// Register Address: 'ha8
#define RX2MDC5_RX_PHASE_COMP_MSK      0x3FF00000
#define RX2MDC5_RX_PHASE_COMP_SHIFT    20
#define RX2MDC5_RX_Q_DCOFS_MSK         0x000FFC00
#define RX2MDC5_RX_Q_DCOFS_SHIFT       10
#define RX2MDC5_RX_I_DCOFS_MSK         0x000003FF
#define RX2MDC5_RX_I_DCOFS_SHIFT       0

// Register Address: 'hac
#define RX2MIMB0_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX2MIMB0_RX_GAIN_COMP_REC_SHIFT    12
#define RX2MIMB0_RX_GAIN_COMP_MSK          0x00000FFF
#define RX2MIMB0_RX_GAIN_COMP_SHIFT        0

// Register Address: 'hb0
#define RX2MIMB1_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX2MIMB1_RX_GAIN_COMP_REC_SHIFT    12
#define RX2MIMB1_RX_GAIN_COMP_MSK          0x00000FFF
#define RX2MIMB1_RX_GAIN_COMP_SHIFT        0

// Register Address: 'hb4
#define RX2MIMB2_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX2MIMB2_RX_GAIN_COMP_REC_SHIFT    12
#define RX2MIMB2_RX_GAIN_COMP_MSK          0x00000FFF
#define RX2MIMB2_RX_GAIN_COMP_SHIFT        0

// Register Address: 'hb8
#define RX2MIMB3_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX2MIMB3_RX_GAIN_COMP_REC_SHIFT    12
#define RX2MIMB3_RX_GAIN_COMP_MSK          0x00000FFF
#define RX2MIMB3_RX_GAIN_COMP_SHIFT        0

// Register Address: 'hbc
#define RX2MIMB4_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX2MIMB4_RX_GAIN_COMP_REC_SHIFT    12
#define RX2MIMB4_RX_GAIN_COMP_MSK          0x00000FFF
#define RX2MIMB4_RX_GAIN_COMP_SHIFT        0

// Register Address: 'hc0
#define RX2MIMB5_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX2MIMB5_RX_GAIN_COMP_REC_SHIFT    12
#define RX2MIMB5_RX_GAIN_COMP_MSK          0x00000FFF
#define RX2MIMB5_RX_GAIN_COMP_SHIFT        0

// Register Address: 'hc4
#define RX4MDC0_RX_PHASE_COMP_MSK      0x3FF00000
#define RX4MDC0_RX_PHASE_COMP_SHIFT    20
#define RX4MDC0_RX_Q_DCOFS_MSK         0x000FFC00
#define RX4MDC0_RX_Q_DCOFS_SHIFT       10
#define RX4MDC0_RX_I_DCOFS_MSK         0x000003FF
#define RX4MDC0_RX_I_DCOFS_SHIFT       0

// Register Address: 'hc8
#define RX4MDC1_RX_PHASE_COMP_MSK      0x3FF00000
#define RX4MDC1_RX_PHASE_COMP_SHIFT    20
#define RX4MDC1_RX_Q_DCOFS_MSK         0x000FFC00
#define RX4MDC1_RX_Q_DCOFS_SHIFT       10
#define RX4MDC1_RX_I_DCOFS_MSK         0x000003FF
#define RX4MDC1_RX_I_DCOFS_SHIFT       0

// Register Address: 'hcc
#define RX4MDC2_RX_PHASE_COMP_MSK      0x3FF00000
#define RX4MDC2_RX_PHASE_COMP_SHIFT    20
#define RX4MDC2_RX_Q_DCOFS_MSK         0x000FFC00
#define RX4MDC2_RX_Q_DCOFS_SHIFT       10
#define RX4MDC2_RX_I_DCOFS_MSK         0x000003FF
#define RX4MDC2_RX_I_DCOFS_SHIFT       0

// Register Address: 'hd0
#define RX4MDC3_RX_PHASE_COMP_MSK      0x3FF00000
#define RX4MDC3_RX_PHASE_COMP_SHIFT    20
#define RX4MDC3_RX_Q_DCOFS_MSK         0x000FFC00
#define RX4MDC3_RX_Q_DCOFS_SHIFT       10
#define RX4MDC3_RX_I_DCOFS_MSK         0x000003FF
#define RX4MDC3_RX_I_DCOFS_SHIFT       0

// Register Address: 'hd4
#define RX4MDC4_RX_PHASE_COMP_MSK      0x3FF00000
#define RX4MDC4_RX_PHASE_COMP_SHIFT    20
#define RX4MDC4_RX_Q_DCOFS_MSK         0x000FFC00
#define RX4MDC4_RX_Q_DCOFS_SHIFT       10
#define RX4MDC4_RX_I_DCOFS_MSK         0x000003FF
#define RX4MDC4_RX_I_DCOFS_SHIFT       0

// Register Address: 'hd8
#define RX4MDC5_RX_PHASE_COMP_MSK      0x3FF00000
#define RX4MDC5_RX_PHASE_COMP_SHIFT    20
#define RX4MDC5_RX_Q_DCOFS_MSK         0x000FFC00
#define RX4MDC5_RX_Q_DCOFS_SHIFT       10
#define RX4MDC5_RX_I_DCOFS_MSK         0x000003FF
#define RX4MDC5_RX_I_DCOFS_SHIFT       0

// Register Address: 'hdc
#define RX4MIMB0_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX4MIMB0_RX_GAIN_COMP_REC_SHIFT    12
#define RX4MIMB0_RX_GAIN_COMP_MSK          0x00000FFF
#define RX4MIMB0_RX_GAIN_COMP_SHIFT        0

// Register Address: 'he0
#define RX4MIMB1_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX4MIMB1_RX_GAIN_COMP_REC_SHIFT    12
#define RX4MIMB1_RX_GAIN_COMP_MSK          0x00000FFF
#define RX4MIMB1_RX_GAIN_COMP_SHIFT        0

// Register Address: 'he4
#define RX4MIMB2_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX4MIMB2_RX_GAIN_COMP_REC_SHIFT    12
#define RX4MIMB2_RX_GAIN_COMP_MSK          0x00000FFF
#define RX4MIMB2_RX_GAIN_COMP_SHIFT        0

// Register Address: 'he8
#define RX4MIMB3_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX4MIMB3_RX_GAIN_COMP_REC_SHIFT    12
#define RX4MIMB3_RX_GAIN_COMP_MSK          0x00000FFF
#define RX4MIMB3_RX_GAIN_COMP_SHIFT        0

// Register Address: 'hec
#define RX4MIMB4_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX4MIMB4_RX_GAIN_COMP_REC_SHIFT    12
#define RX4MIMB4_RX_GAIN_COMP_MSK          0x00000FFF
#define RX4MIMB4_RX_GAIN_COMP_SHIFT        0

// Register Address: 'hf0
#define RX4MIMB5_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX4MIMB5_RX_GAIN_COMP_REC_SHIFT    12
#define RX4MIMB5_RX_GAIN_COMP_MSK          0x00000FFF
#define RX4MIMB5_RX_GAIN_COMP_SHIFT        0

// Register Address: 'hf4
#define RX8MDC0_RX_PHASE_COMP_MSK      0x3FF00000
#define RX8MDC0_RX_PHASE_COMP_SHIFT    20
#define RX8MDC0_RX_Q_DCOFS_MSK         0x000FFC00
#define RX8MDC0_RX_Q_DCOFS_SHIFT       10
#define RX8MDC0_RX_I_DCOFS_MSK         0x000003FF
#define RX8MDC0_RX_I_DCOFS_SHIFT       0

// Register Address: 'hf8
#define RX8MDC1_RX_PHASE_COMP_MSK      0x3FF00000
#define RX8MDC1_RX_PHASE_COMP_SHIFT    20
#define RX8MDC1_RX_Q_DCOFS_MSK         0x000FFC00
#define RX8MDC1_RX_Q_DCOFS_SHIFT       10
#define RX8MDC1_RX_I_DCOFS_MSK         0x000003FF
#define RX8MDC1_RX_I_DCOFS_SHIFT       0

// Register Address: 'hfc
#define RX8MDC2_RX_PHASE_COMP_MSK      0x3FF00000
#define RX8MDC2_RX_PHASE_COMP_SHIFT    20
#define RX8MDC2_RX_Q_DCOFS_MSK         0x000FFC00
#define RX8MDC2_RX_Q_DCOFS_SHIFT       10
#define RX8MDC2_RX_I_DCOFS_MSK         0x000003FF
#define RX8MDC2_RX_I_DCOFS_SHIFT       0

// Register Address: 'h100
#define RX8MDC3_RX_PHASE_COMP_MSK      0x3FF00000
#define RX8MDC3_RX_PHASE_COMP_SHIFT    20
#define RX8MDC3_RX_Q_DCOFS_MSK         0x000FFC00
#define RX8MDC3_RX_Q_DCOFS_SHIFT       10
#define RX8MDC3_RX_I_DCOFS_MSK         0x000003FF
#define RX8MDC3_RX_I_DCOFS_SHIFT       0

// Register Address: 'h104
#define RX8MDC4_RX_PHASE_COMP_MSK      0x3FF00000
#define RX8MDC4_RX_PHASE_COMP_SHIFT    20
#define RX8MDC4_RX_Q_DCOFS_MSK         0x000FFC00
#define RX8MDC4_RX_Q_DCOFS_SHIFT       10
#define RX8MDC4_RX_I_DCOFS_MSK         0x000003FF
#define RX8MDC4_RX_I_DCOFS_SHIFT       0

// Register Address: 'h108
#define RX8MDC5_RX_PHASE_COMP_MSK      0x3FF00000
#define RX8MDC5_RX_PHASE_COMP_SHIFT    20
#define RX8MDC5_RX_Q_DCOFS_MSK         0x000FFC00
#define RX8MDC5_RX_Q_DCOFS_SHIFT       10
#define RX8MDC5_RX_I_DCOFS_MSK         0x000003FF
#define RX8MDC5_RX_I_DCOFS_SHIFT       0

// Register Address: 'h10c
#define RX8MIMB0_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX8MIMB0_RX_GAIN_COMP_REC_SHIFT    12
#define RX8MIMB0_RX_GAIN_COMP_MSK          0x00000FFF
#define RX8MIMB0_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h110
#define RX8MIMB1_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX8MIMB1_RX_GAIN_COMP_REC_SHIFT    12
#define RX8MIMB1_RX_GAIN_COMP_MSK          0x00000FFF
#define RX8MIMB1_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h114
#define RX8MIMB2_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX8MIMB2_RX_GAIN_COMP_REC_SHIFT    12
#define RX8MIMB2_RX_GAIN_COMP_MSK          0x00000FFF
#define RX8MIMB2_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h118
#define RX8MIMB3_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX8MIMB3_RX_GAIN_COMP_REC_SHIFT    12
#define RX8MIMB3_RX_GAIN_COMP_MSK          0x00000FFF
#define RX8MIMB3_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h11c
#define RX8MIMB4_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX8MIMB4_RX_GAIN_COMP_REC_SHIFT    12
#define RX8MIMB4_RX_GAIN_COMP_MSK          0x00000FFF
#define RX8MIMB4_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h120
#define RX8MIMB5_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RX8MIMB5_RX_GAIN_COMP_REC_SHIFT    12
#define RX8MIMB5_RX_GAIN_COMP_MSK          0x00000FFF
#define RX8MIMB5_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h124
#define RXFBDC0_RX_PHASE_COMP_MSK      0x3FF00000
#define RXFBDC0_RX_PHASE_COMP_SHIFT    20
#define RXFBDC0_RX_Q_DCOFS_MSK         0x000FFC00
#define RXFBDC0_RX_Q_DCOFS_SHIFT       10
#define RXFBDC0_RX_I_DCOFS_MSK         0x000003FF
#define RXFBDC0_RX_I_DCOFS_SHIFT       0

// Register Address: 'h128
#define RXFBDC1_RX_PHASE_COMP_MSK      0x3FF00000
#define RXFBDC1_RX_PHASE_COMP_SHIFT    20
#define RXFBDC1_RX_Q_DCOFS_MSK         0x000FFC00
#define RXFBDC1_RX_Q_DCOFS_SHIFT       10
#define RXFBDC1_RX_I_DCOFS_MSK         0x000003FF
#define RXFBDC1_RX_I_DCOFS_SHIFT       0

// Register Address: 'h12c
#define RXFBDC2_RX_PHASE_COMP_MSK      0x3FF00000
#define RXFBDC2_RX_PHASE_COMP_SHIFT    20
#define RXFBDC2_RX_Q_DCOFS_MSK         0x000FFC00
#define RXFBDC2_RX_Q_DCOFS_SHIFT       10
#define RXFBDC2_RX_I_DCOFS_MSK         0x000003FF
#define RXFBDC2_RX_I_DCOFS_SHIFT       0

// Register Address: 'h130
#define RXFBDC3_RX_PHASE_COMP_MSK      0x3FF00000
#define RXFBDC3_RX_PHASE_COMP_SHIFT    20
#define RXFBDC3_RX_Q_DCOFS_MSK         0x000FFC00
#define RXFBDC3_RX_Q_DCOFS_SHIFT       10
#define RXFBDC3_RX_I_DCOFS_MSK         0x000003FF
#define RXFBDC3_RX_I_DCOFS_SHIFT       0

// Register Address: 'h134
#define RXFBDC4_RX_PHASE_COMP_MSK      0x3FF00000
#define RXFBDC4_RX_PHASE_COMP_SHIFT    20
#define RXFBDC4_RX_Q_DCOFS_MSK         0x000FFC00
#define RXFBDC4_RX_Q_DCOFS_SHIFT       10
#define RXFBDC4_RX_I_DCOFS_MSK         0x000003FF
#define RXFBDC4_RX_I_DCOFS_SHIFT       0

// Register Address: 'h138
#define RXFBDC5_RX_PHASE_COMP_MSK      0x3FF00000
#define RXFBDC5_RX_PHASE_COMP_SHIFT    20
#define RXFBDC5_RX_Q_DCOFS_MSK         0x000FFC00
#define RXFBDC5_RX_Q_DCOFS_SHIFT       10
#define RXFBDC5_RX_I_DCOFS_MSK         0x000003FF
#define RXFBDC5_RX_I_DCOFS_SHIFT       0

// Register Address: 'h13c
#define RXFBIMB0_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RXFBIMB0_RX_GAIN_COMP_REC_SHIFT    12
#define RXFBIMB0_RX_GAIN_COMP_MSK          0x00000FFF
#define RXFBIMB0_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h140
#define RXFBIMB1_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RXFBIMB1_RX_GAIN_COMP_REC_SHIFT    12
#define RXFBIMB1_RX_GAIN_COMP_MSK          0x00000FFF
#define RXFBIMB1_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h144
#define RXFBIMB2_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RXFBIMB2_RX_GAIN_COMP_REC_SHIFT    12
#define RXFBIMB2_RX_GAIN_COMP_MSK          0x00000FFF
#define RXFBIMB2_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h148
#define RXFBIMB3_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RXFBIMB3_RX_GAIN_COMP_REC_SHIFT    12
#define RXFBIMB3_RX_GAIN_COMP_MSK          0x00000FFF
#define RXFBIMB3_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h14c
#define RXFBIMB4_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RXFBIMB4_RX_GAIN_COMP_REC_SHIFT    12
#define RXFBIMB4_RX_GAIN_COMP_MSK          0x00000FFF
#define RXFBIMB4_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h150
#define RXFBIMB5_RX_GAIN_COMP_REC_MSK      0x00FFF000
#define RXFBIMB5_RX_GAIN_COMP_REC_SHIFT    12
#define RXFBIMB5_RX_GAIN_COMP_MSK          0x00000FFF
#define RXFBIMB5_RX_GAIN_COMP_SHIFT        0

// Register Address: 'h154
#define RXFILTER_RX_FILTER_HW_CTL_MSK      0x00000010
#define RXFILTER_RX_FILTER_HW_CTL_SHIFT    4
#define RXFILTER_FILTERSEL_MSK             0x00000008
#define RXFILTER_FILTERSEL_SHIFT           3
#define RXFILTER_FILTERBW_MSK              0x00000006
#define RXFILTER_FILTERBW_SHIFT            1
#define RXFILTER_FILTER_EN_MSK             0x00000001
#define RXFILTER_FILTER_EN_SHIFT           0

// Register Address: 'h158
#define TXDIGPWR01_TX_DIG_GAIN1_MSK      0x07FF0000
#define TXDIGPWR01_TX_DIG_GAIN1_SHIFT    16
#define TXDIGPWR01_TX_DIG_GAIN0_MSK      0x000007FF
#define TXDIGPWR01_TX_DIG_GAIN0_SHIFT    0

// Register Address: 'h15c
#define TXDIGPWR23_TX_DIG_GAIN3_MSK      0x07FF0000
#define TXDIGPWR23_TX_DIG_GAIN3_SHIFT    16
#define TXDIGPWR23_TX_DIG_GAIN2_MSK      0x000007FF
#define TXDIGPWR23_TX_DIG_GAIN2_SHIFT    0

// Register Address: 'h160
#define TXDIGPWR45_TX_DIG_GAIN5_MSK      0x07FF0000
#define TXDIGPWR45_TX_DIG_GAIN5_SHIFT    16
#define TXDIGPWR45_TX_DIG_GAIN4_MSK      0x000007FF
#define TXDIGPWR45_TX_DIG_GAIN4_SHIFT    0

int32 hgrfdigicali_attach(uint32 dev_id, struct hgrfdigicali *prfdigicali);

#endif
