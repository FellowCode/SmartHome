from django.core.mail import EmailMultiAlternatives
from SmartHome.settings import ALLOWED_HOSTS

admin_emails = ['sergo79_f1@mail.ru']

from_email = 'info@guraton.ru'

def send_confirm_mail(user):
    subject = 'Подтверждение почты'
    text_content = 'Для использования функционала сервиса необходимо подтвердить почту. \n'
    text_content += 'Ссылка подтверждения почты: http://{0}/account/emailconfirm/{1}/{2}/ \n'.format(ALLOWED_HOSTS[0],
                                                                                                     user.id,
                                                                                                     user.extendeduser.token)
    text_content += 'Если вы получили письмо по ошибке, вы можете его проигнорировать.\n'

    html_content = '<p>Для использования функционала сервиса необходимо подтвердить почту</p>'
    html_content += '<p>Ссылка подтверждения почты: http://{0}/account/emailconfirm/{1}/{2}/</p>'.format(ALLOWED_HOSTS[0],
                                                                                                     user.id,
                                                                                                     user.extendeduser.token)
    html_content += '<p>Если вы получили письмо по ошибке, вы можете его проигнорировать.</p>'

    msg = EmailMultiAlternatives(subject, text_content, from_email, [user.email])
    msg.attach_alternative(html_content, "text/html")
    msg.send(fail_silently=False)


def send_restore_pass_mail(user):
    subject = 'Восстановление пароля'
    text_content = 'Вы отправили запрос на восстановление пароля.\n'
    text_content += 'Для восстановления перейдите по ссылке: http://{0}/account/restore/confirm/{1}/{2}/ \n'.format(ALLOWED_HOSTS[0],
                                                                                                     user.id,

                                                                                                     user.extendeduser.restore_token)
    text_content += 'Если вы получили письмо по ошибке, вы можете его проигнорировать.\n'

    html_content = '<p>Вы отправили запрос на восстановление пароля.</p>'
    html_content += '<p>Для восстановления перейдите по ссылке: http://{0}/account/restore/confirm/{1}/{2}/ </p>'.format(ALLOWED_HOSTS[0],
                                                                                                     user.id,

                                                                                                     user.extendeduser.restore_token)
    html_content += '<p>Если вы получили письмо по ошибке, вы можете его проигнорировать.</p>'

    msg = EmailMultiAlternatives(subject, text_content, from_email, [user.email])
    msg.attach_alternative(html_content, "text/html")
    msg.send(fail_silently=False)
