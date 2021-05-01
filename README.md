RC araçlara trainer portü üzerinden Joystick bağlamak üzerine yapılmış bir çalışmadır.

Proje tanıtım videosu : https://youtu.be/n3OITI3CZNc

Yazılımın çalışması için PPMEncoder kütüphanesi Arduino'ya eklenmelidir.
PPMEncoder: https://github.com/schinken/PPMEncoder
Üst menülerden Taslak > library ekle > .ZIP Kitaplığı Ekle kısmına girilip üstteki linkten indirilen zip dosyası seçilmelidir.

Donanım konusunda bilgilendirme:
*Sistem potansiyometrelerdeki gerilimi ölçerek konumu öğrenmek ve trim değerlerini ölçülen değerlere ekleyerek çıkış vermek üzerine kurulmuştur.

*Joystick mekaniğinde yer alan potansiyometreler şemaya uygun şekilde bağlanmalıdır.

*Butonlar için pulldown dirençleri önemlidir. Sistemin kararlı çalışması için pulldown dirençleri kullanılmalıdır. 1K - 10K aralığında direnç kullanılabilir. (Şemada 10K olarak belirtilmiştir.)

*AUX1 ve AUX 2 için 1K - 10K arasında potansiyometre kullanarak analog çıkış elde edebilirsiniz. Yerlerine buton veya iki konumlu switch'i AUX3 veya AUX4'teki gibi bağlayarak da kullanabilirsiniz.

*AUX3 ve AUX4 için şemada buton bağlantısı yapılmıştır. Buton yerine iki konumlu switch de kullanılabilir.

*10. pinden çıkış sinyali PPM olarak verilmektedir. Kumandanıza uygun soket kullanarak sinyali bağlayabilirsiniz. Birbirinden bağımsız iki elektronik devrenin çalışması için Joystick'in

*GND pini ile kumandanın GND pinini birleştirmeniz gerektiğini unutmayın.(Trainer soketinde GND pini bulunmaktadır.)12. pine buzzer bağlayarak trim ve otokalibrasyon mekanizmasının sesli bildirimlerini dinleyebilirsiniz.

Ek bilgiler:
*Aileron ve elevator dışındaki tüm girişler opsiyoneldir. İstenildiğinde konfigürasyon bölümüne // eklenerek kapatılabilir.

Otokalibrasyon:
*Aieleron trim butonlarının her ikisine de basılarak elektrik verildiğinde kesintisiz buzzer sesi duyulur. 10 saniye boyunca duyulacak olan bu ses kalibrasyonun başladığını belirtir. Aileron,
elevator , throttle (tanımlı ise) , rudder (tanımlı ise) max ve minimum noktalarına getirilir. Yazılım max ve min noktalarını hafızaya kaydeder. 10 saniye sonunda buzzer sesi kesilir ve 2 saniye
boyunca tekrar aktif hale gelir. Bu aşamada eksenler (throttle hariç) merkez noktada bırakılır ve yazılıma eksenlerin merkez noktası tanımlanmış olur. 2 saniye dolduğunda kalibrasyon 
tamamlanır ve yazılım çalışmaya başlar.

Teşekkürler:
*Oğuzhan Taşdemir'e ve INMUK üyelerine çalışmada verdiği destekten dolayı teşekkürler.

İletişim:

*Soru, öneri veya hata bildirmek isterseniz aşağıdaki davet linkinden Telegram grubumuza ulaşarak bildirebilirsiniz.

Telegram grubu: https://t.me/joinchat/S-207kyqjQuEK85VETLKsA

MIT lisansı ile paylaşılan Christopher Schirner (schinken)'in PPMEncoder kütüphanesi kullanılarak PPM çıkışı sağlanmıştır.
