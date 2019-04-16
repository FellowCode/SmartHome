from django.contrib import admin
from django.urls import path, include
from .views import *

urlpatterns = [
    path('signup/', signup),
    path('signin/', signin),
    path('logout/', user_logout),
    path('emailconfirm/again/', email_confirm_again),
    path('emailconfirm/<user_id>/<token>/', email_confirm),
    path('restore/confirm/<user_id>/<token>/', restore_pass_confirm),
    path('restore/', restore_password),
    path('', user_account)
]