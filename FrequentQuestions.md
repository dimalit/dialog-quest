**Как установить BOOST?**

Под Windows удобнее всего воспользоваться бинарным дистрибутивом: http://www.boostpro.com/download/ (качать последнюю версию 1.51.0)
К сожалению, проект BoostPro закончен, и в процессе использования скачанной exe-шки могут возникнуть проблемы с "зеркалами скачивания". Если в процессе инсталляции возникнут ошибки, связанные с "недоступностью сервера" следует попробовать выбрать другое зеркало для скачивания. При настройке дистрибутива в качестве необходимых типов библиотек выбирайте (как минимум) "static" и "static debug". Учтите также, что каждая "галочка" "весит" около 250 Мб.

Открою секрет: на самом деле в проекте из boost'а используется только boost::signals. Это "header-only library" и поэтому можно просто выудить boost/signal.hpp (и, наверное, некоторые другие файлы, которые он подключает) с сайта boost.org и подкинуть его (их) в папку svn/shared/util/boost/boost. Также из этого следует, что при использовании бинарного инсталлятора в окошке выбора устанавливаемых библиотек достаточно выбрать только signals (библиотеки-зависимости оно установит самостоятельно).

**Не найден boost/config.hpp. Что делать?**

Видимо, вы неправильно прописали переменную кружения BOOST\_ROOT. Запустите командную строку (пуск->выполнить->cmd) и напишите:
```
echo %BOOST_ROOT%
```
На экране должно появиться:
```
C:\Program Files\boost
```
(или другой путь, куда Вы установили boost). Главное - это должна быть та папка, в которой есть еще две подпапки: boost и lib. Если тут все OK - значит надо перезапустить Visual Studio, чтобы она "увидела" эти изменения.

**Ошибка при запуске!!!**

См. вывод на консоль. Если там ничего интересного - см. log.txt. На всякий случай напоминаю запустить файл update\_media.bat из папки ClassContainer/media (в целях диагностики можно запустить его из консоли (cmd) и проверить, что все успешно отработало). Кстати, саму exe-шку программы (winRTSimpleApp.exe) тоже можно запустить из-под cmd, чтобы увидеть, что она выведет на экран.

**Как добавить свой графический файл?**
Поместие файл в формате bmp или png в одну из подпапок папки media и запустите update\_media.bat (interface - для элементов, которые нужны везде, game - на отдельных сценах). Этот скрипт переконвертирует ваш файл в формат .rttex и поместит его в соответствующую подпапку папки bin. Найдите этот файл! В вашем lua-скрипте используйте имя "path/name.rttex" для его загрузки. Файлы jpg копируйте сразу в нужное место в папке bin и используйте непосредственно.

**Как менять цвет шрифта?**
Прямо в тексте можно "на лету" менять цвет с помощью специальных кодов. Например, стрка "здра`1в`0ствуйте" будет напечатана белым цветом с синей буквой "в" (мы использовали код цвета 1 для включения синего цвета, а затем код цвета 0 для возврата к белому). Какие еще бывают коды - см. media/interface/font\_times.txt (в самом конце файла).