// -----------------------------------------------------------------------------
// 'smartdac' Register Definitions
// Revision: 163
// -----------------------------------------------------------------------------
// Generated on 2019-12-12 at 01:49 (UTC) by airhdl version 2019.09.1
// -----------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
// -----------------------------------------------------------------------------

#ifndef SMARTDAC_REGS_H
#define SMARTDAC_REGS_H

/* Revision number of the 'smartdac' register map */
#define SMARTDAC_REVISION 163

/* Default base address of the 'smartdac' register map */
#define SMARTDAC_DEFAULT_BASEADDR 0x80000000

/* Register 'bitstream' */
#define BITSTREAM_OFFSET 0x00000000 /* address offset of the 'bitstream' register */

/* Field  'bitstream.value' */
#define BITSTREAM_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define BITSTREAM_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define BITSTREAM_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define BITSTREAM_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'revision' */
#define REVISION_OFFSET 0x00000004 /* address offset of the 'revision' register */

/* Field  'revision.value' */
#define REVISION_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define REVISION_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define REVISION_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define REVISION_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'dietemp' */
#define DIETEMP_OFFSET 0x00000008 /* address offset of the 'dietemp' register */

/* Field  'dietemp.value' */
#define DIETEMP_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define DIETEMP_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define DIETEMP_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define DIETEMP_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'wdctrl' */
#define WDCTRL_OFFSET 0x0000000C /* address offset of the 'wdctrl' register */

/* Field  'wdctrl.value' */
#define WDCTRL_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define WDCTRL_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define WDCTRL_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define WDCTRL_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'triggers' */
#define TRIGGERS_OFFSET 0x00000010 /* address offset of the 'triggers' register */

/* Field  'triggers.value' */
#define TRIGGERS_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define TRIGGERS_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define TRIGGERS_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define TRIGGERS_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'tripctrl' */
#define TRIPCTRL_OFFSET 0x00000020 /* address offset of the 'tripctrl' register */

/* Field  'tripctrl.value' */
#define TRIPCTRL_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define TRIPCTRL_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define TRIPCTRL_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define TRIPCTRL_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'tripmask' */
#define TRIPMASK_OFFSET 0x00000024 /* address offset of the 'tripmask' register */
#define TRIPMASK_ARRAY_LENGTH 2 /* length of the 'tripmask' register array, in elements */

/* Field  'tripmask.value' */
#define TRIPMASK_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define TRIPMASK_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define TRIPMASK_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define TRIPMASK_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'tripsts' */
#define TRIPSTS_OFFSET 0x0000002C /* address offset of the 'tripsts' register */
#define TRIPSTS_ARRAY_LENGTH 2 /* length of the 'tripsts' register array, in elements */

/* Field  'tripsts.value' */
#define TRIPSTS_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define TRIPSTS_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define TRIPSTS_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define TRIPSTS_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'tripsrc' */
#define TRIPSRC_OFFSET 0x00000034 /* address offset of the 'tripsrc' register */
#define TRIPSRC_ARRAY_LENGTH 2 /* length of the 'tripsrc' register array, in elements */

/* Field  'tripsrc.value' */
#define TRIPSRC_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define TRIPSRC_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define TRIPSRC_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define TRIPSRC_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pwmen' */
#define PWMEN_OFFSET 0x00000040 /* address offset of the 'pwmen' register */

/* Field  'pwmen.value' */
#define PWMEN_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PWMEN_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PWMEN_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PWMEN_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pwmsts' */
#define PWMSTS_OFFSET 0x00000044 /* address offset of the 'pwmsts' register */

/* Field  'pwmsts.value' */
#define PWMSTS_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PWMSTS_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PWMSTS_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PWMSTS_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pwmred' */
#define PWMRED_OFFSET 0x00000048 /* address offset of the 'pwmred' register */

/* Field  'pwmred.value' */
#define PWMRED_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PWMRED_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PWMRED_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PWMRED_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pwmfed' */
#define PWMFED_OFFSET 0x0000004C /* address offset of the 'pwmfed' register */

/* Field  'pwmfed.value' */
#define PWMFED_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PWMFED_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PWMFED_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PWMFED_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pwminc' */
#define PWMINC_OFFSET 0x00000050 /* address offset of the 'pwminc' register */

/* Field  'pwminc.value' */
#define PWMINC_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PWMINC_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PWMINC_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PWMINC_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pwmphase' */
#define PWMPHASE_OFFSET 0x00000054 /* address offset of the 'pwmphase' register */
#define PWMPHASE_ARRAY_LENGTH 16 /* length of the 'pwmphase' register array, in elements */

/* Field  'pwmphase.value' */
#define PWMPHASE_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PWMPHASE_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PWMPHASE_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PWMPHASE_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'ctrlinterval' */
#define CTRLINTERVAL_OFFSET 0x00000100 /* address offset of the 'ctrlinterval' register */

/* Field  'ctrlinterval.value' */
#define CTRLINTERVAL_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define CTRLINTERVAL_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define CTRLINTERVAL_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define CTRLINTERVAL_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'ctrllatsync' */
#define CTRLLATSYNC_OFFSET 0x00000104 /* address offset of the 'ctrllatsync' register */

/* Field  'ctrllatsync.value' */
#define CTRLLATSYNC_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define CTRLLATSYNC_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define CTRLLATSYNC_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define CTRLLATSYNC_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'digitalo' */
#define DIGITALO_OFFSET 0x00000110 /* address offset of the 'digitalo' register */

/* Field  'digitalo.value' */
#define DIGITALO_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define DIGITALO_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define DIGITALO_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define DIGITALO_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'digitali' */
#define DIGITALI_OFFSET 0x00000114 /* address offset of the 'digitali' register */

/* Field  'digitali.value' */
#define DIGITALI_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define DIGITALI_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define DIGITALI_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define DIGITALI_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'auxo' */
#define AUXO_OFFSET 0x00000120 /* address offset of the 'auxo' register */
#define AUXO_ARRAY_LENGTH 16 /* length of the 'auxo' register array, in elements */

/* Field  'auxo.value' */
#define AUXO_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define AUXO_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define AUXO_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define AUXO_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'auxi' */
#define AUXI_OFFSET 0x00000160 /* address offset of the 'auxi' register */
#define AUXI_ARRAY_LENGTH 16 /* length of the 'auxi' register array, in elements */

/* Field  'auxi.value' */
#define AUXI_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define AUXI_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define AUXI_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define AUXI_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'sercomtx' */
#define SERCOMTX_OFFSET 0x00000200 /* address offset of the 'sercomtx' register */
#define SERCOMTX_ARRAY_LENGTH 8 /* length of the 'sercomtx' register array, in elements */

/* Field  'sercomtx.value' */
#define SERCOMTX_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define SERCOMTX_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define SERCOMTX_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define SERCOMTX_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'sercomrx' */
#define SERCOMRX_OFFSET 0x00000280 /* address offset of the 'sercomrx' register */
#define SERCOMRX_ARRAY_LENGTH 8 /* length of the 'sercomrx' register array, in elements */

/* Field  'sercomrx.value' */
#define SERCOMRX_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define SERCOMRX_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define SERCOMRX_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define SERCOMRX_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'ledctrl' */
#define LEDCTRL_OFFSET 0x00000300 /* address offset of the 'ledctrl' register */
#define LEDCTRL_ARRAY_LENGTH 16 /* length of the 'ledctrl' register array, in elements */

/* Field  'ledctrl.value' */
#define LEDCTRL_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define LEDCTRL_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define LEDCTRL_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define LEDCTRL_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'netscopeba' */
#define NETSCOPEBA_OFFSET 0x000B0000 /* address offset of the 'netscopeba' register */

/* Field  'netscopeba.value' */
#define NETSCOPEBA_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define NETSCOPEBA_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define NETSCOPEBA_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define NETSCOPEBA_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'netscopepl' */
#define NETSCOPEPL_OFFSET 0x000B0004 /* address offset of the 'netscopepl' register */

/* Field  'netscopepl.value' */
#define NETSCOPEPL_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define NETSCOPEPL_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define NETSCOPEPL_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define NETSCOPEPL_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'netscopedl' */
#define NETSCOPEDL_OFFSET 0x000B0008 /* address offset of the 'netscopedl' register */

/* Field  'netscopedl.value' */
#define NETSCOPEDL_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define NETSCOPEDL_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define NETSCOPEDL_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define NETSCOPEDL_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'netscopebl' */
#define NETSCOPEBL_OFFSET 0x000B000C /* address offset of the 'netscopebl' register */

/* Field  'netscopebl.value' */
#define NETSCOPEBL_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define NETSCOPEBL_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define NETSCOPEBL_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define NETSCOPEBL_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'netscopeto' */
#define NETSCOPETO_OFFSET 0x000B0010 /* address offset of the 'netscopeto' register */

/* Field  'netscopeto.value' */
#define NETSCOPETO_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define NETSCOPETO_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define NETSCOPETO_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define NETSCOPETO_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pmonctrl' */
#define PMONCTRL_OFFSET 0x000C0000 /* address offset of the 'pmonctrl' register */

/* Field  'pmonctrl.value' */
#define PMONCTRL_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PMONCTRL_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PMONCTRL_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PMONCTRL_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pmonsts' */
#define PMONSTS_OFFSET 0x000C0004 /* address offset of the 'pmonsts' register */

/* Field  'pmonsts.value' */
#define PMONSTS_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PMONSTS_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PMONSTS_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PMONSTS_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pmonutilmax' */
#define PMONUTILMAX_OFFSET 0x000C0100 /* address offset of the 'pmonutilmax' register */

/* Field  'pmonutilmax.value' */
#define PMONUTILMAX_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PMONUTILMAX_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PMONUTILMAX_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PMONUTILMAX_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pmonutilmin' */
#define PMONUTILMIN_OFFSET 0x000C0104 /* address offset of the 'pmonutilmin' register */

/* Field  'pmonutilmin.value' */
#define PMONUTILMIN_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PMONUTILMIN_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PMONUTILMIN_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PMONUTILMIN_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pmonutilavg' */
#define PMONUTILAVG_OFFSET 0x000C0108 /* address offset of the 'pmonutilavg' register */

/* Field  'pmonutilavg.value' */
#define PMONUTILAVG_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PMONUTILAVG_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PMONUTILAVG_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PMONUTILAVG_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pmonlatmax' */
#define PMONLATMAX_OFFSET 0x000C0200 /* address offset of the 'pmonlatmax' register */

/* Field  'pmonlatmax.value' */
#define PMONLATMAX_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PMONLATMAX_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PMONLATMAX_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PMONLATMAX_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pmonlatmin' */
#define PMONLATMIN_OFFSET 0x000C0204 /* address offset of the 'pmonlatmin' register */

/* Field  'pmonlatmin.value' */
#define PMONLATMIN_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PMONLATMIN_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PMONLATMIN_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PMONLATMIN_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'pmonlatavg' */
#define PMONLATAVG_OFFSET 0x000C0208 /* address offset of the 'pmonlatavg' register */

/* Field  'pmonlatavg.value' */
#define PMONLATAVG_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define PMONLATAVG_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define PMONLATAVG_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define PMONLATAVG_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'logctrl' */
#define LOGCTRL_OFFSET 0x000D0000 /* address offset of the 'logctrl' register */

/* Field  'logctrl.value' */
#define LOGCTRL_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define LOGCTRL_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define LOGCTRL_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define LOGCTRL_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'logsts' */
#define LOGSTS_OFFSET 0x000D0004 /* address offset of the 'logsts' register */

/* Field  'logsts.value' */
#define LOGSTS_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define LOGSTS_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define LOGSTS_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define LOGSTS_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'logvar' */
#define LOGVAR_OFFSET 0x000D0008 /* address offset of the 'logvar' register */
#define LOGVAR_ARRAY_LENGTH 16 /* length of the 'logvar' register array, in elements */

/* Field  'logvar.value' */
#define LOGVAR_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define LOGVAR_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define LOGVAR_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define LOGVAR_VALUE_RESET 0x0 /* reset value of the 'value' field */

/* Register 'logdata' */
#define LOGDATA_OFFSET 0x000D0050 /* address offset of the 'logdata' register */
#define LOGDATA_DEPTH 262144 /* depth of the 'logdata' memory, in elements */

/* Field  'logdata.value' */
#define LOGDATA_VALUE_BIT_OFFSET 0 /* bit offset of the 'value' field */
#define LOGDATA_VALUE_BIT_WIDTH 32 /* bit width of the 'value' field */
#define LOGDATA_VALUE_BIT_MASK 0xFFFFFFFF /* bit mask of the 'value' field */
#define LOGDATA_VALUE_RESET 0x0 /* reset value of the 'value' field */

#endif  /* SMARTDAC_REGS_H */
