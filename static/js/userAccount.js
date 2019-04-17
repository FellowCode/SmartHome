var csrf;
$(document).ready(function () {
    csrf = $('input[name=csrfmiddlewaretoken]').val();
    var keyHide = true;

    listItemInit();

    $('a#keys-show').click(function () {
        if (keyHide) {
            $(this).parent().find('.keys').css('display', 'block');
            $(this).html('Скрыть ключи');
        }
        else {
            $(this).parent().find('.keys').css('display', 'none');
            $(this).html('Показать ключи');
        }
        keyHide = !keyHide;
    });

    var timerId = setInterval(updateDeviceData, 10000);

    var notify = $('.green-notification');
    if (notify.length){
        setTimeout(animateClose.bind(null, notify), 5000);
    }

});

function sendChangeRequest(deviceBlock) {
    var id = deviceBlock.data('id');
    var enable = deviceBlock.find('input[name=enable]').prop('checked');
    var target_temp = deviceBlock.find('input[name=target_temp]').val();

    var data = {'csrfmiddlewaretoken': csrf, 'id': id, 'enable': enable, 'target_temp': target_temp};

    var post = $.post("/device/change/", data);
}

function listItemInit() {
    $('input[name=target_temp]').focusout(function () {
        sendChangeRequest($(this).closest('.device'));
    });
    $('input[name=enable]').change(function () {
        sendChangeRequest($(this).closest('.device'));
    });
}

function updateDeviceData() {
    $('#device-list').load("/account/ #device-list-result", listItemInit);
}

function animateClose(element){
    $(element).animate({height: 0}, 1000, function () {
        $(this).remove()
    })
}