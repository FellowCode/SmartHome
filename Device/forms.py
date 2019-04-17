from django import forms
from django.contrib.auth.models import User


class SettingsForm(forms.Form):
    custom_name = forms.CharField(max_length=100)

    def clean_custom_name(self):
        name = self.cleaned_data['custom_name']
        if len(name) < 1:
            self.add_error('custom_name', 'Не менее 1 символа')
        elif self.forbidden_sym(name):
            self.add_error('custom_name', 'Недопустимый символ в имени')
        return name

    def forbidden_sym(self, value):
        forb_sym = ';: '
        for char in forb_sym:
            if char in value:
                return True
        return False
