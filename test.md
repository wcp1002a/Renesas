```
    // Bit1: Channel Halt Status Flag 
    PrintText("\n\r");
    PrintText("CAN-FD self-mode test:\n\r");

    ctr = RCFDC0CFDC3CTR;
    sprintf(tx_buf, "    RCFDC0CFDC3CTR: %08X\n\r", ctr);
    PrintText_delay(tx_buf, 10);

    sts = RCFDC0CFDC3STS;
    sprintf(tx_buf, "    RCFDC0CFDC3STS: %08X\n\r", sts);
    PrintText_delay(tx_buf, 10);
```

### test
    
Title22
----------------------------------------
<pre>
```
    // Bit1: Channel Halt Status Flag 
    PrintText("\n\r");
    PrintText("CAN-FD self-mode test:\n\r");

    ctr = RCFDC0CFDC3CTR;
    sprintf(tx_buf, "    RCFDC0CFDC3CTR: %08X\n\r", ctr);
    PrintText_delay(tx_buf, 10);

    sts = RCFDC0CFDC3STS;
    sprintf(tx_buf, "    RCFDC0CFDC3STS: %08X\n\r", sts);
    PrintText_delay(tx_buf, 10);
```
</pre>

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