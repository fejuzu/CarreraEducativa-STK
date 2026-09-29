//
// Carrera Educativa STK - demo question bank
// GPL v3 or later.
//

#include "education/demo_questions.hpp"

namespace Education
{
namespace
{
Question makeQuestion(int id, const std::string& topic,
                      const std::string& subtopic,
                      const std::string& prompt,
                      const std::vector<std::string>& options,
                      std::size_t correct_index,
                      const std::string& explanation,
                      QuestionType type = QuestionType::MULTIPLE_CHOICE)
{
    Question q;
    q.id = id;
    q.level = 1;
    q.type = type;
    q.topic = topic;
    q.subtopic = subtopic;
    q.prompt = prompt;
    q.options = options;
    q.correct_index = correct_index;
    q.explanation = explanation;
    return q;
}
}

std::vector<Question> createDemoQuestionBank()
{
    std::vector<Question> q;
    q.reserve(20);

    q.push_back(makeQuestion(1, "Seguridad en obra", "EPP",
        "¿Qué elemento protege principalmente la cabeza frente a golpes o caída de objetos?",
        {"Casco de seguridad", "Guantes", "Chaleco reflectivo", "Botas"},
        0, "El casco reduce el riesgo de lesiones en la cabeza."));

    q.push_back(makeQuestion(2, "Seguridad en obra", "Señalización",
        "Una señal circular roja con una acción tachada normalmente indica:",
        {"Obligación", "Prohibición", "Información", "Ruta de evacuación"},
        1, "Las señales de prohibición suelen usar círculo rojo y una barra diagonal."));

    q.push_back(makeQuestion(3, "Seguridad en obra", "Orden y limpieza",
        "Mantener libres los pasillos de trabajo ayuda principalmente a evitar:",
        {"Caídas y tropiezos", "Ruido excesivo", "Radiación", "Cambios de temperatura"},
        0, "El orden y la limpieza reducen caídas al mismo nivel."));

    q.push_back(makeQuestion(4, "Seguridad en obra", "Altura",
        "Antes de usar una escalera portátil se debe:",
        {"Correr con ella", "Revisar su estado y apoyo", "Subir de espaldas", "Apoyarla sobre cajas"},
        1, "La inspección y un apoyo estable son controles básicos antes de usarla."));

    q.push_back(makeQuestion(5, "Seguridad en obra", "Electricidad",
        "¿Qué debes hacer antes de intervenir un equipo eléctrico?",
        {"Mojar el área", "Aumentar la carga", "Desenergizar y verificar", "Retirar la puesta a tierra"},
        2, "La intervención segura requiere desenergización y verificación de ausencia de tensión."));

    q.push_back(makeQuestion(6, "Seguridad en obra", "EPP",
        "Los lentes de seguridad sirven principalmente para proteger:",
        {"Los ojos", "Los oídos", "Las rodillas", "La espalda"},
        0, "Los lentes protegen frente a partículas, polvo y salpicaduras según el modelo."));

    q.push_back(makeQuestion(7, "Seguridad en obra", "Herramientas",
        "Una herramienta con cable eléctrico dañado debe:",
        {"Seguir usándose con cuidado", "Retirarse de servicio", "Mojarse", "Prestarse a otro trabajador"},
        1, "Un cable dañado puede producir choque eléctrico o incendio."));

    q.push_back(makeQuestion(8, "Seguridad en obra", "Excavaciones",
        "En una excavación, el material retirado debe mantenerse:",
        {"Al borde", "A distancia segura del borde", "Dentro de la excavación", "Sobre la escalera"},
        1, "Alejar el material reduce sobrecarga del borde y caída de objetos."));

    q.push_back(makeQuestion(9, "Seguridad en obra", "Emergencias",
        "Si detectas humo o fuego fuera de control, lo prioritario es:",
        {"Ocultarlo", "Dar la alarma y seguir el plan de emergencia", "Continuar trabajando", "Cerrar todas las salidas"},
        1, "La respuesta debe seguir el plan de emergencia y las rutas establecidas."));

    q.push_back(makeQuestion(10, "Seguridad en obra", "Manipulación manual",
        "Al levantar una carga manualmente conviene:",
        {"Girar el tronco mientras se levanta", "Mantener la carga cerca del cuerpo", "Levantar con una sola mano", "Doblar solo la espalda"},
        1, "Mantener la carga cerca reduce el esfuerzo sobre la espalda."));

    q.push_back(makeQuestion(11, "Seguridad en obra", "EPP",
        "¿Verdadero o falso? El EPP reemplaza todos los demás controles de seguridad.",
        {"Verdadero", "Falso"},
        1, "El EPP es una barrera adicional; no reemplaza controles de ingeniería, administrativos u otros.",
        QuestionType::TRUE_FALSE));

    q.push_back(makeQuestion(12, "Seguridad en obra", "Ruido",
        "En una zona con nivel de ruido peligroso se debe usar:",
        {"Protección auditiva adecuada", "Solo chaleco", "Solo lentes", "Ningún EPP"},
        0, "La protección auditiva debe seleccionarse según la exposición al ruido."));

    q.push_back(makeQuestion(13, "Seguridad en obra", "Tránsito interno",
        "Al caminar cerca de maquinaria móvil es mejor:",
        {"Usar rutas peatonales señalizadas", "Caminar detrás sin avisar", "Usar audífonos", "Cruzar por puntos ciegos"},
        0, "Las rutas segregadas disminuyen el riesgo de atropello."));

    q.push_back(makeQuestion(14, "Seguridad en obra", "Andamios",
        "Antes de trabajar sobre un andamio se debe comprobar:",
        {"Que esté estable y habilitado", "Que tenga ruedas sueltas", "Que falten barandas", "Que esté inclinado"},
        0, "El andamio debe estar correctamente montado e inspeccionado."));

    q.push_back(makeQuestion(15, "Seguridad en obra", "Químicos",
        "Para conocer peligros y medidas de un producto químico se consulta:",
        {"La hoja de datos de seguridad", "La tarjeta de asistencia", "El calendario", "El plano de estacionamiento"},
        0, "La SDS/HDS contiene información de peligros, manejo y respuesta a emergencias."));

    q.push_back(makeQuestion(16, "Seguridad en obra", "Caídas",
        "¿Verdadero o falso? Un piso húmedo sin señalizar puede generar riesgo de caída.",
        {"Verdadero", "Falso"},
        0, "Las superficies húmedas pueden reducir la adherencia y causar resbalones.",
        QuestionType::TRUE_FALSE));

    q.push_back(makeQuestion(17, "Seguridad en obra", "Equipos",
        "Antes de operar una máquina que no conoces debes:",
        {"Improvisar", "Recibir autorización/capacitación correspondiente", "Desactivar protecciones", "Aumentar la velocidad"},
        1, "La operación debe realizarse por personal autorizado y capacitado."));

    q.push_back(makeQuestion(18, "Seguridad en obra", "Trabajo en equipo",
        "Si observas una condición insegura grave debes:",
        {"Ignorarla", "Reportarla y aplicar el procedimiento correspondiente", "Ocultarla", "Esperar varios días"},
        1, "Reportar oportunamente permite controlar el peligro antes de un incidente."));

    q.push_back(makeQuestion(19, "Seguridad en obra", "Señalización",
        "El color verde en señalización de seguridad suele asociarse con:",
        {"Prohibición", "Equipos contra incendio", "Condición segura o evacuación", "Peligro eléctrico exclusivamente"},
        2, "El verde suele identificar condiciones seguras, primeros auxilios o rutas de evacuación."));

    q.push_back(makeQuestion(20, "Seguridad en obra", "Prevención",
        "La acción más efectiva ante un peligro es, cuando sea posible:",
        {"Eliminar el peligro", "Ignorarlo", "Depender solo del EPP", "Aumentar la exposición"},
        0, "Eliminar el peligro se encuentra en la parte superior de la jerarquía de controles."));

    return q;
}

} // namespace Education
