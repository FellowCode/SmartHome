google.charts.load('current', {'packages': ['corechart']});
var templist = [];
var humlist = [];
$(document).ready(function () {
    $('#statistic>li').each(function (index, item) {
        templist.push([new Date($(item).text()), parseFloat($(item).data('temp'))]);
        humlist.push([new Date($(item).text()), parseFloat($(item).data('hum'))]);
    });
    google.charts.setOnLoadCallback(drawChart);
});

function drawChart() {
    var data_temp = new google.visualization.DataTable();
    data_temp.addColumn('date', 'Time of Day');
    data_temp.addColumn('number', 'Температура');
    data_temp.addRows(templist);

    var options_temp = {
        title: 'Температура',
        curveType: 'function',
        hAxis: {
            format: 'MM.dd',
            gridlines: {count: 15}
        },
        legend: {position: 'bottom'}
    };

    var chart_temp = new google.visualization.LineChart(document.getElementById('curve_chart_temp'));
    chart_temp.draw(data_temp, options_temp);

    var data_humidity = new google.visualization.DataTable();
    data_humidity.addColumn('date', 'Time of Day');
    data_humidity.addColumn('number', 'Влажность');
    data_humidity.addRows(humlist);

    var options_humidity = {
        title: 'Влажность',
        curveType: 'function',
        hAxis: {
            format: 'MM.dd',
            gridlines: {count: 15}
        },
        legend: {position: 'bottom'}
    };

    var chart_humidity = new google.visualization.LineChart(document.getElementById('curve_chart_humidity'));
    chart_humidity.draw(data_humidity, options_humidity);
}