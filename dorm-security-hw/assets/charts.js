(function () {
  var style = getComputedStyle(document.documentElement);
  var accent = style.getPropertyValue('--accent').trim();
  var accent2 = style.getPropertyValue('--accent2').trim();
  var ink = style.getPropertyValue('--ink').trim();
  var muted = style.getPropertyValue('--muted').trim();
  var rule = style.getPropertyValue('--rule').trim();
  var bg2 = style.getPropertyValue('--bg2').trim();

  var el = document.getElementById('chart-power');
  if (!el || typeof echarts === 'undefined') return;

  var chart = echarts.init(el, null, { renderer: 'svg' });

  var rows = [
    { name: 'MQ-2 烟雾', value: 150, rail: '5V' },
    { name: 'ESP-01S 峰值', value: 300, rail: '3.3V' },
    { name: 'Blue Pill', value: 50, rail: '5V' },
    { name: '蜂鸣器', value: 30, rail: '3.3V' },
    { name: 'OLED', value: 25, rail: '3.3V' },
    { name: 'LED ×2', value: 8, rail: '3.3V' },
    { name: '火焰 / 光敏 / DHT11', value: 11, rail: '3.3V' }
  ];

  chart.setOption({
    animation: false,
    grid: { left: 130, right: 78, top: 18, bottom: 34 },
    tooltip: {
      trigger: 'axis',
      appendToBody: true,
      backgroundColor: bg2,
      borderColor: rule,
      textStyle: { color: ink, fontFamily: 'Instrument Sans, Microsoft YaHei, sans-serif', fontSize: 12 },
      formatter: function (params) {
        var p = params[0];
        var row = rows.filter(function (r) { return r.name === p.name; })[0];
        return p.name + '<br/><b>' + p.value + ' mA</b>　' + (row ? row.rail + ' 轨' : '');
      }
    },
    xAxis: {
      type: 'value',
      max: 340,
      name: 'mA',
      nameTextStyle: { color: muted, fontSize: 10 },
      axisLabel: { color: muted, fontSize: 10 },
      axisLine: { lineStyle: { color: rule } },
      splitLine: { lineStyle: { color: rule, type: 'dashed' } }
    },
    yAxis: {
      type: 'category',
      inverse: true,
      data: rows.map(function (r) { return r.name; }),
      axisLabel: { color: ink, fontSize: 11, fontFamily: 'Instrument Sans, Microsoft YaHei, sans-serif' },
      axisLine: { lineStyle: { color: rule } },
      axisTick: { show: false }
    },
    series: [{
      type: 'bar',
      barWidth: 16,
      data: rows.map(function (r) {
        return {
          value: r.value,
          itemStyle: { color: r.rail === '5V' ? accent : accent2, borderRadius: [0, 3, 3, 0] }
        };
      }),
      label: {
        show: true,
        position: 'right',
        color: muted,
        fontSize: 10,
        formatter: '{c} mA'
      }
    }]
  });

  window.addEventListener('resize', function () { chart.resize(); });
})();
