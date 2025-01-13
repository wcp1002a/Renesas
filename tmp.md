tmp
===
aa

```asm
#pragma inline_asm      asm_get_pc
static int      asm_get_pc(void){
        mov 4, r2
        mov lp, r10
        sub r2, r10
}
```

```c
/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
uint32_t P_1MS_COUNTER;
int32_t pTAUD0_CH3_cmp_value;
int32_t pwm_chg = 1;
/* End user code. Do not edit comment generated here */
void r_main_userinit(void);

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
    r_main_userinit();
    /* Start user code for main. Do not edit comment generated here */
    R_Config_TAUB0_0_Start();
    R_Config_TAUD0_Start();
    pTAUD0_CH3_cmp_value = _TAUD0_CHANNEL3_COMPARE_VALUE;
    pDelayMs(1000);
    while(1){
        pDelayMs(100);
        if(pTAUD0_CH3_cmp_value > _TAUD0_CHANNEL0_COMPARE_VALUE - pwm_chg){
            pwm_chg = -pwm_chg;
        }else if(pTAUD0_CH3_cmp_value < -pwm_chg){
            pwm_chg = -pwm_chg;
        }
        pTAUD0_CH3_cmp_value += pwm_chg;
        TAUD0.CDR3 = pTAUD0_CH3_cmp_value;
    }
    /* End user code. Do not edit comment generated here */
}
```
11testa

<style>
pre {
  display: block;
  margin-top: 0;
  margin-bottom: 1rem;
  font-size: 0.7rem;
  line-height: 1.4;
  white-space: pre;
  overflow: auto;
  background-color: #f9f9f9;
  border: 1px solid #ddd;
  padding: .5rem;
  max-height: 800px;
  font-family: monospace;
  code {
    color: inherit;
    background-color: transparent;
    padding: 0;
    display: block;
  }
  .line-number {
    display: block;
    float: left;
    margin: 0 1em 0 -1em;
    border-right: 1px solid #ddd;
    text-align: right;
    span {
      display: block;
      padding: 0 .5em 0 1em;
      color: #ccc;
    }
  }
  .cl {
    display: block;
    clear: both;
  }
}
</style>

<script type="text/javascript">
(function() {
  var pre = document.getElementsByTagName('pre'),
    pl = pre.length;
  for (var i = 0; i < pl; i++) {
    pre[i].innerHTML = '<span class="line-number"></span>' + pre[i].innerHTML + '<span class="cl"></span>';
    var num = pre[i].innerHTML.split(/\n/).length;
    for (var j = 0; j < (num - 1); j++) {
      var line_num = pre[i].getElementsByTagName('span')[0];
      line_num.innerHTML += '<span>' + (j + 1) + '</span>';
    }
  }
})();
</script>
