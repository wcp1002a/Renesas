test1
=====

```javascript {.line-numbers}
function add(x, y) {
  return x + y;
}
```


```{.javascript .numberLines .lineAnchors}
var s = "JavaScript syntax highlighting";
alert(s);
```
q
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

<script>
(function() {
  var pre = document.getElementsByTagName('pre'), pl = pre.length;
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

<style>
pre {
  display: block;
}
pre code {
  display: block;
}
pre .line-number {
  display: block;
  float: left;
  margin: 0 1em 0 -1em;
  border-right: 1px solid #aaa;
  text-align: right;
  padding:5px 0;
}
pre .line-number span {
  display: block;
  padding: 0 .5em 0 1em;
  color: #aaaaaa;
  line-height: 19px;
  font-size: 14px;
}
pre .cl {
  display: block;
  clear: both;
}
</style>
