from telegram import Update
from telegram.ext import Application, CommandHandler, ContextTypes

TOKEN = "8766086948:AAFF6_gx-taFdKpN1zUTV8zfcqtT8an2yA4"  # сюда ВСТАВЬ НОВЫЙ токен

MESSAGE_TEXT = "ГОЙДААА"


async def send_periodic(context: ContextTypes.DEFAULT_TYPE) -> None:
    chat_id = context.job.chat_id
    await context.bot.send_message(chat_id=chat_id, text=MESSAGE_TEXT)


async def start(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    chat_id = update.effective_chat.id

    # Удаляем старые задачи для этого чата (если были)
    jobs = context.job_queue.get_jobs_by_name(str(chat_id))
    for job in jobs:
        job.schedule_removal()

    # Запускаем задачу: каждые 60 секунд
    context.job_queue.run_repeating(
        send_periodic,
        interval=0.5,   # каждые 60 секунд
        first=0,       # начать сразу
        chat_id=chat_id,
        name=str(chat_id),
    )

    await update.message.reply_text("Запустил ГОЙДААА каждую минуту.")


async def stop(update: Update, context: ContextTypes.DEFAULT_TYPE) -> None:
    chat_id = update.effective_chat.id
    jobs = context.job_queue.get_jobs_by_name(str(chat_id))
    if not jobs:
        await update.message.reply_text("Нечего останавливать.")
        return
    for job in jobs:
        job.schedule_removal()
    await update.message.reply_text("Остановил ГОЙДААА.")


def main() -> None:
    app = Application.builder().token(TOKEN).build()

    app.add_handler(CommandHandler("start", start))
    app.add_handler(CommandHandler("stop", stop))

    app.run_polling()


if __name__ == "__main__":
    main()