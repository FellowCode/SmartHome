from django import forms
from django.contrib.auth.models import User

class SignInForm(forms.Form):
    email = forms.EmailField()
    password = forms.CharField()

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # if you want to do it to all of them
        for field in self.fields.values():
            field.error_messages = {'required': 'Это обязательное поле', 'invalid': 'Проверьте правильность'}


class SignUpForm(forms.Form):
    first_name = forms.CharField(max_length=256)
    last_name = forms.CharField(max_length=256)
    email = forms.EmailField()
    password = forms.CharField()
    password_confirm = forms.CharField()

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        # if you want to do it to all of them
        for field in self.fields.values():
            field.error_messages = {'required': 'Это обязательное поле', 'invalid': 'Проверьте правильность'}

    def clean_first_name(self):
        f_name = self.cleaned_data['first_name']
        if len(f_name) < 2:
            self.add_error('first_name', 'Не менее 2 символов')
        elif self.forbidden_sym(f_name):
            self.add_error('first_name', 'Недопустимый символ в имени')
        return f_name

    def clean_last_name(self):
        l_name = self.cleaned_data['last_name']
        if len(l_name) < 2:
            self.add_error('first_name', 'Не менее 2 символов')
        elif self.forbidden_sym(l_name):
            self.add_error('last_name', 'Недопустимый символ в фамилии')
        return l_name

    def clean_email(self):
        email = self.cleaned_data['email']
        if User.objects.filter(username=email).exists():
            self.add_error('email', 'Такой email уже зарегистрирован')
        return email


    def clean_password(self):
        password = self.cleaned_data['password']
        if len(password) < 8:
            self.add_error('password', 'В пароле менее 8 символов')
        return password

    def clean_password_confirm(self):
        password = self.cleaned_data['password']
        password_confirm = self.cleaned_data['password_confirm']
        if password != password_confirm:
            self.add_error('password_confirm', 'Пароли не совпадают')

    def forbidden_sym(self, value):
        forb_sym = ';: '
        for char in forb_sym:
            if char in value:
                return True
        return False

class RestorePassForm(forms.Form):
    email = forms.EmailField()

    def clean_email(self):
        email = self.cleaned_data['email']
        user = User.objects.filter(username=email).first()
        if not user:
            self.add_error('email', 'Такой email не зарегистрирован')
        return email

class RestorePassConfirmForm(forms.Form):
    password = forms.CharField(max_length=256)
    password_confirm = forms.CharField(max_length=256)

    def clean_password(self):
        password = self.cleaned_data['password']
        if len(password) < 8:
            self.add_error('password', 'В пароле менее 8 символов')
        return password

    def clean_password_confirm(self):
        password = self.cleaned_data['password']
        password_confirm = self.cleaned_data['password_confirm']
        if password != password_confirm:
            self.add_error('password_confirm', 'Пароли не совпадают')
