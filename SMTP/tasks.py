from django.core.mail import EmailMultiAlternatives
from SmartHome.settings import ALLOWED_HOSTS

admin_emails = ['sergo79_f1@mail.ru']


def send_confirm_mail(user):
    subject, from_email = 'Подтверждение почты', 'info@apex79.ru'
    text_content = 'Для использования функционала сервиса необходимо подтвердить почту'
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
