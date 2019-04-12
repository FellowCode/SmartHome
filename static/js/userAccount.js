var csrf;
$(document).ready(function () {
    csrf = $('input[name=csrfmiddlewaretoken]').val();

    listItemInit();

    var timerId = setInterval(updateDeviceData, 10000);
});

function sendChangeRequest(deviceBlock) {
    var id = deviceBlock.data('id');
    var enable = deviceBlock.find('input[name=enable]').prop('checked');
    var target_temp = deviceBlock.find('input[name=target_temp]').val();

    var data = {'csrfmiddlewaretoken': csrf, 'id': id, 'enable': enable, 'target_temp': target_temp};

    var post = $.post("/device/change/", data);
}
function listItemInit(){
    $('input[name=target_temp]').focusout(function () {
        sendChangeRequest($(this).closest('.device'));
    });
    $('input[name=enable]').change(function () {
        sendChangeRequest($(this).closest('.device'));
    });
}
function updateDeviceData() {
    $('#device-list').load("/account/ #device-list", listItemInit);
}