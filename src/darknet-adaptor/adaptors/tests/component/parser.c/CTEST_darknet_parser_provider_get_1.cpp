#include "CTEST_darknet_parser_provider.h"

const dlinfov& CTEST_darknet_parser_provider::get_1()
{
  static float layer_0_biases[] = {
      -0.114799947f, 0.706177533f,  0.292092592f,  -0.00620544376f,
      -0.204626665f, -0.210669905f, -0.170483887f, -0.160724819f,
  };
  static size_t layer_0_biases_size = 8;
  static float* layer_0_scales = nullptr;
  static size_t layer_0_scales_size = 0U;
  static float* layer_0_rolling_mean = nullptr;
  static size_t layer_0_rolling_mean_size = 0U;
  static float* layer_0_rolling_variance = nullptr;
  static size_t layer_0_rolling_variance_size = 0U;
  static float layer_0_weights[] = {
      -0.504392087f, 0.379526556f, -0.135891601f, 0.280939728f,
      0.123974063f,  0.181649983f, -0.555455446f, -0.0965111554f,
  };
  static size_t layer_0_weights_size = 8;
  static float* layer_1_biases = nullptr;
  static size_t layer_1_biases_size = 0U;
  static float* layer_1_scales = nullptr;
  static size_t layer_1_scales_size = 0U;
  static float* layer_1_rolling_mean = nullptr;
  static size_t layer_1_rolling_mean_size = 0U;
  static float* layer_1_rolling_variance = nullptr;
  static size_t layer_1_rolling_variance_size = 0U;
  static float* layer_1_weights = nullptr;
  static size_t layer_1_weights_size = 0U;
  static float layer_2_biases[] = {
      -0.0237044692f,  -0.0387823582f, 0.129725918f,   -0.0249654744f,
      -0.00263095065f, 0.208563253f,   0.266139448f,   0.0371248573f,
      -0.00674681319f, 0.110266432f,   0.0885364264f,  0.0881028175f,
      0.0556188747f,   0.29175061f,    -0.0539293066f, -0.0386850201f,
  };
  static size_t layer_2_biases_size = 16;
  static float* layer_2_scales = nullptr;
  static size_t layer_2_scales_size = 0U;
  static float* layer_2_rolling_mean = nullptr;
  static size_t layer_2_rolling_mean_size = 0U;
  static float* layer_2_rolling_variance = nullptr;
  static size_t layer_2_rolling_variance_size = 0U;
  static float layer_2_weights[] = {
      0.053542722f,  -0.272461951f, -0.0478333309f, -0.0175146461f,
      0.0563009158f, -0.128462911f, -0.0826644078f, -0.0545044541f,
      -0.184921354f, -0.11511948f,  -0.144535333f,  -0.0721389651f,
      0.0887186602f, 0.101050749f,  0.0311767329f,  0.0206814073f,
  };
  static size_t layer_2_weights_size = 16;
  static float* layer_3_biases = nullptr;
  static size_t layer_3_biases_size = 0U;
  static float* layer_3_scales = nullptr;
  static size_t layer_3_scales_size = 0U;
  static float* layer_3_rolling_mean = nullptr;
  static size_t layer_3_rolling_mean_size = 0U;
  static float* layer_3_rolling_variance = nullptr;
  static size_t layer_3_rolling_variance_size = 0U;
  static float* layer_3_weights = nullptr;
  static size_t layer_3_weights_size = 0U;
  static float layer_4_biases[] = {
      -0.00943185948f, -0.130857915f,  0.248232678f,   0.283543259f,
      -0.0217314474f,  0.0242004152f,  0.0363860503f,  0.116776176f,
      -0.0971987024f,  0.0641403347f,  -0.0826910883f, -0.0561241508f,
      -0.091162838f,   -0.084923923f,  0.0533067808f,  -0.0161047745f,
      0.0251615494f,   -0.0409073047f, 0.137330905f,   0.166604117f,
      0.0815394744f,   -0.038728781f,  0.079000324f,   -0.0776125416f,
      0.0895529687f,   -0.137916118f,  -0.126963899f,  -0.0538230017f,
      -0.0409904383f,  -0.0925081f,    -0.0667603537f, 0.112727813f,
  };
  static size_t layer_4_biases_size = 32;
  static float* layer_4_scales = nullptr;
  static size_t layer_4_scales_size = 0U;
  static float* layer_4_rolling_mean = nullptr;
  static size_t layer_4_rolling_mean_size = 0U;
  static float* layer_4_rolling_variance = nullptr;
  static size_t layer_4_rolling_variance_size = 0U;
  static float layer_4_weights[] = {
      0.0621511228f,  -0.140142575f,  -0.0691301376f,  -0.128702506f,
      0.0634062365f,  -0.0889573395f, 0.000775506254f, -0.136339247f,
      -0.0673711076f, -0.0283758137f, 0.031909842f,    0.0999220833f,
      0.0333535746f,  0.0718126521f,  0.109128751f,    -0.0434606895f,
      -0.0506336577f, 0.0057387799f,  0.0750331283f,   -0.0675663128f,
      -0.0575703532f, 0.0618861392f,  0.026299648f,    0.0146402577f,
      -0.0106604081f, 0.0259510037f,  -0.00701069133f, -0.0258505102f,
      0.0615007505f,  0.00777151855f, -0.00360565702f, -0.00418959558f,
  };
  static size_t layer_4_weights_size = 32;
  static float* layer_5_biases = nullptr;
  static size_t layer_5_biases_size = 0U;
  static float* layer_5_scales = nullptr;
  static size_t layer_5_scales_size = 0U;
  static float* layer_5_rolling_mean = nullptr;
  static size_t layer_5_rolling_mean_size = 0U;
  static float* layer_5_rolling_variance = nullptr;
  static size_t layer_5_rolling_variance_size = 0U;
  static float* layer_5_weights = nullptr;
  static size_t layer_5_weights_size = 0U;
  static float layer_6_biases[] = {
      0.024220014f,   0.224382371f,   0.223398328f,     -0.00895210821f,
      0.0362842903f,  0.131112546f,   -0.000176672984f, 0.0451514311f,
      0.365529776f,   0.0643663555f,  0.0399540775f,    0.062388774f,
      0.0104296133f,  0.00431534369f, 0.256032288f,     -0.00987227634f,
      0.0471100248f,  0.0209819265f,  0.0187062118f,    0.15494141f,
      0.0164312534f,  0.300182909f,   0.0873807147f,    0.658025622f,
      -0.0203492846f, 0.167788148f,   0.3189601f,       0.025858365f,
      0.196658924f,   0.0338891633f,  0.0229186453f,    0.361172289f,
      0.0100069838f,  0.0917698443f,  0.0154303834f,    -0.0292860437f,
      0.0332427472f,  0.189777598f,   0.0593688525f,    0.0278129932f,
      0.32389608f,    0.366229892f,   -0.0120334867f,   -0.00486083701f,
      0.104770966f,   0.025330171f,   0.239379436f,     0.466013879f,
      0.0765731931f,  0.0416647494f,  0.238509223f,     0.0126562836f,
      0.0812146664f,  0.00705742696f, 0.24944064f,      -0.00134043687f,
      0.00611999305f, 0.0857662857f,  0.00496091647f,   0.00504026422f,
      -0.0525321625f, -0.0137029681f, 0.229435876f,     0.0639654398f,
  };
  static size_t layer_6_biases_size = 64;
  static float* layer_6_scales = nullptr;
  static size_t layer_6_scales_size = 0U;
  static float* layer_6_rolling_mean = nullptr;
  static size_t layer_6_rolling_mean_size = 0U;
  static float* layer_6_rolling_variance = nullptr;
  static size_t layer_6_rolling_variance_size = 0U;
  static float layer_6_weights[] = {
      0.153157324f,   -0.27456522f,   0.134155884f,    -0.186962858f,
      0.0867453143f,  -0.0828833282f, 0.156955957f,    0.0634316355f,
      0.144412979f,   0.176086172f,   -0.223237395f,   0.125185475f,
      -0.100996234f,  -0.230260998f,  0.115947224f,    0.09353026f,
      -0.199014232f,  0.211967319f,   -0.177188516f,   -0.340500027f,
      -0.188226163f,  0.0848638266f,  0.0553097539f,   0.0747910887f,
      0.071064949f,   0.0215391554f,  0.000515697524f, -0.159678474f,
      -0.182101071f,  -0.0271459594f, -0.145424783f,   -0.0264421087f,
      0.179299831f,   -0.348480165f,  0.181992546f,    0.0919719785f,
      -0.0485647954f, -0.0347482041f, -0.0710493177f,  -0.0654570833f,
      -0.110334918f,  -0.0286384914f, -0.307453305f,   0.186860546f,
      -0.118419357f,  0.0836696178f,  0.021372525f,    0.0160281081f,
      -0.0571226142f, 0.117154919f,   0.102328897f,    0.077316992f,
      -0.0156702865f, -0.0379628614f, 0.0151991323f,   -0.254719555f,
      0.161519602f,   -0.114893027f,  0.03134178f,     -0.101617053f,
      -0.5239622f,    -0.0357279554f, 0.242745265f,    0.175372303f,
  };
  static size_t layer_6_weights_size = 64;
  static float* layer_7_biases = nullptr;
  static size_t layer_7_biases_size = 0U;
  static float* layer_7_scales = nullptr;
  static size_t layer_7_scales_size = 0U;
  static float* layer_7_rolling_mean = nullptr;
  static size_t layer_7_rolling_mean_size = 0U;
  static float* layer_7_rolling_variance = nullptr;
  static size_t layer_7_rolling_variance_size = 0U;
  static float* layer_7_weights = nullptr;
  static size_t layer_7_weights_size = 0U;
  static float layer_8_biases[] = {
      -0.0138007114f, 0.0101061473f,  -0.0353558324f, -0.0766393244f,
      -0.601125181f,  -0.132182419f,  -0.0259236787f, -0.136161536f,
      -0.149978146f,  -0.147952005f,  -0.185334235f,  -0.151727855f,
      -0.11818511f,   -0.0941482112f, -0.195221364f,  -0.216732785f,
      0.0f,           0.0f,           0.0f,           0.0f,
      -0.492853105f,  0.0f,           0.0f,           0.0f,
      0.0f,           0.0f,           0.0f,           0.0f,
      0.0f,           0.0f,           0.0f,           0.0f,
      0.0f,           0.0f,           0.0f,           0.0f,
      -0.505641937f,  0.0f,           0.0f,           0.0f,
      0.0f,           0.0f,           0.0f,           0.0f,
      0.0f,           0.0f,           0.0f,           0.0f,
      0.0f,           0.0f,           0.0f,           0.0f,
      -0.50756222f,   0.0f,           0.0f,           0.0f,
      0.0f,           0.0f,           0.0f,           0.0f,
      0.0f,           0.0f,           0.0f,           0.0f,
  };
  static size_t layer_8_biases_size = 64;
  static float* layer_8_scales = nullptr;
  static size_t layer_8_scales_size = 0U;
  static float* layer_8_rolling_mean = nullptr;
  static size_t layer_8_rolling_mean_size = 0U;
  static float* layer_8_rolling_variance = nullptr;
  static size_t layer_8_rolling_variance_size = 0U;
  static float layer_8_weights[] = {
      0.0486010127f,  0.0252603516f,   0.0316057689f,   0.00617672782f,
      0.00956111308f, -0.0534065887f,  0.0367207043f,   0.0243385416f,
      0.0288977288f,  0.0640859827f,   0.0134599656f,   0.0345835313f,
      0.00628267229f, -0.00197592867f, -0.0209349543f,  -0.045077458f,
      0.0229735617f,  -0.0320775956f,  0.00138908904f,  -0.0138669321f,
      0.00968218409f, 0.0385184921f,   0.032845512f,    -0.0161790103f,
      0.0433700234f,  0.0164594688f,   0.000838104461f, 0.00159636769f,
      -0.0119582796f, -0.0493173227f,  -0.00768282032f, 0.06100896f,
      0.00171889004f, -0.0243023671f,  0.00592119899f,  -0.026168447f,
      -0.0995842963f, 0.045079045f,    0.0161695238f,   0.00312854908f,
      -0.0402731448f, -0.0152330054f,  -0.0262564179f,  -0.026314741f,
      -0.0388272554f, 0.00634514494f,  -0.0469579361f,  0.0199052915f,
      -0.0575655326f, 0.0579825714f,   0.0235789232f,   0.0459040888f,
      0.0355221853f,  0.0185159594f,   0.0255434159f,   -0.043253459f,
      -0.0300949328f, 0.0120829809f,   0.033477664f,    0.0101316767f,
      0.00547352526f, -0.0218168404f,  0.00506807677f,  -0.0931278318f,
  };
  static size_t layer_8_weights_size = 64;
  static float layer_9_biases[] = {
      32.f,
      64.f,
  };
  static size_t layer_9_biases_size = 2;
  static float* layer_9_scales = nullptr;
  static size_t layer_9_scales_size = 0U;
  static float* layer_9_rolling_mean = nullptr;
  static size_t layer_9_rolling_mean_size = 0U;
  static float* layer_9_rolling_variance = nullptr;
  static size_t layer_9_rolling_variance_size = 0U;
  static float* layer_9_weights = nullptr;
  static size_t layer_9_weights_size = 0U;

  static const std::vector<dlinfo> d_1{
      dlinfo{
          0,
          layer_0_biases,
          layer_0_biases_size,
          layer_0_scales,
          layer_0_scales_size,
          layer_0_rolling_mean,
          layer_0_rolling_mean_size,
          layer_0_rolling_variance,
          layer_0_rolling_variance_size,
          layer_0_weights,
          layer_0_weights_size,
      },

      dlinfo{
          3,
          layer_1_biases,
          layer_1_biases_size,
          layer_1_scales,
          layer_1_scales_size,
          layer_1_rolling_mean,
          layer_1_rolling_mean_size,
          layer_1_rolling_variance,
          layer_1_rolling_variance_size,
          layer_1_weights,
          layer_1_weights_size,
      },

      dlinfo{
          0,
          layer_2_biases,
          layer_2_biases_size,
          layer_2_scales,
          layer_2_scales_size,
          layer_2_rolling_mean,
          layer_2_rolling_mean_size,
          layer_2_rolling_variance,
          layer_2_rolling_variance_size,
          layer_2_weights,
          layer_2_weights_size,
      },

      dlinfo{
          3,
          layer_3_biases,
          layer_3_biases_size,
          layer_3_scales,
          layer_3_scales_size,
          layer_3_rolling_mean,
          layer_3_rolling_mean_size,
          layer_3_rolling_variance,
          layer_3_rolling_variance_size,
          layer_3_weights,
          layer_3_weights_size,
      },

      dlinfo{
          0,
          layer_4_biases,
          layer_4_biases_size,
          layer_4_scales,
          layer_4_scales_size,
          layer_4_rolling_mean,
          layer_4_rolling_mean_size,
          layer_4_rolling_variance,
          layer_4_rolling_variance_size,
          layer_4_weights,
          layer_4_weights_size,
      },

      dlinfo{
          3,
          layer_5_biases,
          layer_5_biases_size,
          layer_5_scales,
          layer_5_scales_size,
          layer_5_rolling_mean,
          layer_5_rolling_mean_size,
          layer_5_rolling_variance,
          layer_5_rolling_variance_size,
          layer_5_weights,
          layer_5_weights_size,
      },

      dlinfo{
          0,
          layer_6_biases,
          layer_6_biases_size,
          layer_6_scales,
          layer_6_scales_size,
          layer_6_rolling_mean,
          layer_6_rolling_mean_size,
          layer_6_rolling_variance,
          layer_6_rolling_variance_size,
          layer_6_weights,
          layer_6_weights_size,
      },

      dlinfo{
          3,
          layer_7_biases,
          layer_7_biases_size,
          layer_7_scales,
          layer_7_scales_size,
          layer_7_rolling_mean,
          layer_7_rolling_mean_size,
          layer_7_rolling_variance,
          layer_7_rolling_variance_size,
          layer_7_weights,
          layer_7_weights_size,
      },

      dlinfo{
          0,
          layer_8_biases,
          layer_8_biases_size,
          layer_8_scales,
          layer_8_scales_size,
          layer_8_rolling_mean,
          layer_8_rolling_mean_size,
          layer_8_rolling_variance,
          layer_8_rolling_variance_size,
          layer_8_weights,
          layer_8_weights_size,
      },

      dlinfo{
          28,
          layer_9_biases,
          layer_9_biases_size,
          layer_9_scales,
          layer_9_scales_size,
          layer_9_rolling_mean,
          layer_9_rolling_mean_size,
          layer_9_rolling_variance,
          layer_9_rolling_variance_size,
          layer_9_weights,
          layer_9_weights_size,
      },
  };  // d_1

  return d_1;
}
